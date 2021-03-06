#include <mvp/Image/OrbitalImage.h>

#include <vw/FileIO/DiskImageView.h>
#include <vw/Image/Algorithms.h>
#include <vw/Image/ImageViewRef.h>
#include <vw/Image/Interpolation.h>

#include <vw/Camera/PinholeModel.h>

#include <boost/foreach.hpp>

namespace mvp {
namespace image {

OrbitalImage::OrbitalImage(OrbitalImageDesc const& desc) {
  vw::BBox2i cropbox(desc.minx(), desc.miny(), desc.width(), desc.height());

  vw::DiskImageView<OrbitalImagePixel> rsrc(desc.image_path());
  m_image = vw::crop(rsrc, cropbox);

  m_camera = vw::camera::crop(vw::camera::PinholeModel(desc.camera_path()), cropbox);
}

// TODO: write any type of camera
OrbitalImageDesc OrbitalImage::write(std::string const& prefix) const {
  OrbitalImageDesc desc;
  desc.set_image_path(prefix + ".tif");
  desc.set_camera_path(prefix + ".pinhole");
  desc.set_minx(0);
  desc.set_miny(0);
  desc.set_width(m_image.cols());
  desc.set_height(m_image.rows());

  vw::write_image(desc.image_path(), m_image);
  m_camera.write(desc.camera_path());

  return desc;
}

vw::ImageView<OrbitalImagePixel> OrbitalImage::back_project(vw::Vector3 const& xyz, 
                                                      vw::Quat const& orientation,
                                                      vw::Vector2 const& scale,
                                                      vw::Vector2i const& size) const
{
  using namespace vw;

  vw::ImageView<OrbitalImagePixel> patch(size.x(), size.y());
  Matrix3x3 morientation = orientation.rotation_matrix();

  for (int row = 0; row < size.y(); row++) {
    for (int col = 0; col < size.x(); col++) {
      Vector2 patch_pt = elem_prod(Vector2(col, row) - Vector2(size) / 2, scale);
      Vector2 image_pt = m_camera.point_to_pixel(morientation * Vector3(patch_pt.x(), patch_pt.y(), 0) + xyz);

      patch(col, row) = vw::interpolate(m_image, BilinearInterpolation(), ZeroEdgeExtension())(image_pt.x(), image_pt.y());
    }
  }

  return patch;
}

}} // namespace image,mvp
