#include <iostream>

#include <vw/Image/ImageMath.h>
#include <vw/Image/UtilityViews.h>
#include <vw/Plate/PlateFile.h>

#include <mvp/MVPWorkspace.h>
#include <mvp/MVPJob.h>

#include <boost/program_options.hpp>

using namespace vw;
using namespace vw::cartography;
using namespace vw::platefile;
using namespace std;
using namespace mvp;

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  po::options_description cmd_opts("Command line options");
  cmd_opts.add_options()
    ("help,h", "Print this message")
    ("silent", "Run without outputting status")
    ("config-file,f", po::value<string>()->default_value("mvp.conf"), "Specify a pipeline configuration file")
    ;

  po::options_description mvp_opts;
  mvp_opts.add(MVPWorkspace::program_options());

  po::options_description all_opts;
  all_opts.add(cmd_opts).add(mvp_opts);

  po::variables_map vm;
  store(po::command_line_parser(argc, argv).options(all_opts).run(), vm);

  if (vm.count("help")) {
    cout << all_opts << endl;
    return 0;
  }

  ifstream ifs(vm["config-file"].as<string>().c_str());
  if (ifs) {
    store(parse_config_file(ifs, mvp_opts), vm);
  }

  notify(vm);


  MVPWorkspace work(MVPWorkspace::construct_from_program_options(vm)); 

  /*
  const int MAX_OVERLAP = 4;

  PlateGeoReference plate_georef(Datum("D_MOON"), "equi", 256, GeoReference::PixelAsPoint);
  MVPAlgorithmSettings settings;
  settings.set_test_algorithm(true);

  MVPWorkspace work("result.plate", "result-internal.plate", plate_georef, settings);
  work.add_image_pattern("synth.%d.pinhole", "synth.%d.tif", Vector2i(0, 3));

  int render_level = work.equal_resolution_level();
  BBox2i tile_bbox(work.tile_work_area(render_level));

  cout << tile_bbox << endl;
  cout << work.pixel_work_area(render_level) << endl;

  boost::shared_ptr<PlateFile> pf(new PlateFile("output.plate", "equi", "desc", plate_georef.tile_size(), "tif", VW_PIXEL_RGBA, VW_CHANNEL_UINT8));
  Transaction tid = pf->transaction_request("tdesc", -1);

  for (int col = tile_bbox.min().x(); col < tile_bbox.max().x(); col++) {
    for (int row = tile_bbox.min().y(); row < tile_bbox.max().y(); row++) {
      MVPJobRequest job = work.assemble_job(col, row, render_level);
      MVPTileProcessor proc(job);
      MVPTileResult result = proc.process();

      ImageView<PixelRGBA<uint8> > rendered_tile = pixel_cast_rescale<PixelRGB<uint8> >(result.post_height / MAX_OVERLAP);

      pf->write_request();
      pf->write_update(rendered_tile, col, row, render_level, tid);
      pf->sync();
      pf->write_complete();
  
      cout << "Wrote tile: " << col << " " << row << " " << render_level << endl;
    }
  }

  for (int level = 2; level < render_level; level++) {
    int divisor = render_level - level;
    for (int col = tile_bbox.min().x() >> divisor; col <= tile_bbox.max().x() >> divisor; col++) {
      for (int row = tile_bbox.min().y() >> divisor; row <= tile_bbox.max().y() >> divisor; row++) {
        ImageView<PixelRGBA<uint8> > rendered_tile(constant_view(PixelRGBA<uint8>(255, 0, 0, 255), plate_georef.tile_size(), plate_georef.tile_size()));
        pf->write_request();
        pf->write_update(rendered_tile, col, row, level, tid);
        pf->sync();
        pf->write_complete();
      }
    }
  }
  */
  return 0;
}
