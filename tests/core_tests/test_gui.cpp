/***************************************************************************
*   Copyright (C) 2005-2007 by the FIFE Team                              *
*   fife-public@lists.sourceforge.net                                     *
*   This file is part of FIFE.                                            *
*                                                                         *
*   FIFE is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
***************************************************************************/

// Standard C++ library includes

// 3rd party library includes
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <SDL.h>
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfssourcefactory.h"
#include "vfs/vfs.h"
#include "util/rect.h"
#include "util/settingsmanager.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "vfs/raw/rawdata.h"
#include "video/image_location.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/opengl/renderbackendopengl.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "loaders/native/video_loaders/subimage_provider.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "util/exception.h"
#include "gui/base/opengl/opengl_gui_graphics.h"
#include "gui/base/sdl/sdl_gui_graphics.h"
#include "gui/base/gui_image.h"
#include "gui/base/gui_imageloader.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string IMAGE_FILE = "../../content/gfx/tiles/beach/beach_e1.png";
static const std::string SUBIMAGE_FILE = "../../content/gfx/tiles/rpg_tiles_01.png";

// Environment
struct environment {
	boost::shared_ptr<SettingsManager> settings;
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;

	environment()
		: settings(new SettingsManager()),
		  timemanager(new TimeManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()) {
		VFS::instance()->addSource(new VFSHostSystem());
			if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
				throw SDLException(SDL_GetError());
			}
		}
};

void test_gui_image(RenderBackend& renderbackend, gcn::Graphics& graphics, ImagePool& pool, SDL_Surface* screen) {
	pool.addResourceProvider(new SubImageProvider());
	pool.addResourceProvider(new ImageProvider());

	GuiImageLoader imageloader(pool);
	gcn::Image::setImageLoader(&imageloader);	

	gcn::Container* top = new gcn::Container();
	top->setDimension(gcn::Rectangle(0, 0, 200, 300));
	gcn::Gui* gui = new gcn::Gui();
	gui->setGraphics(&graphics);
	gui->setTop(top);
	gcn::Label* label = new gcn::Label("Label");
;	gcn::Image* guiimage = gcn::Image::load(IMAGE_FILE);
	gcn::Icon* icon = new gcn::Icon(guiimage);
 
	top->add(label, 10, 10);
	top->add(icon, 10, 30);

	ImageProvider provider;
	boost::scoped_ptr<Image> img(provider.createImage(ImageLocation(IMAGE_FILE)));
	
	int h = img->getHeight();
	int w = img->getWidth();
	for (int i = 0; i < 400; i+=2) {
		renderbackend.startFrame();
		img->render(Rect(i, i, w, h), screen);
		gui->logic();
		gui->draw();
		renderbackend.endFrame();
	}	
}

void test_sdl_gui_image() {
	environment env;
	RenderBackendSDL renderbackend;
	renderbackend.init();
	ImagePool pool;
	SDL_Surface* screen = renderbackend.createMainScreen(800, 600, 0, false);
	SdlGuiGraphics graphics(pool);
	graphics.setTarget(screen);
	test_gui_image(renderbackend, graphics, pool, screen);
}

void test_ogl_gui_image() {
	environment env;
	RenderBackendOpenGL renderbackend;
	renderbackend.init();
	ImagePool pool;
	SDL_Surface* screen = renderbackend.createMainScreen(800, 600, 0, false);
	OpenGLGuiGraphics graphics(pool);
	test_gui_image(renderbackend, graphics, pool, screen);
}

test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Gui Tests");
	test->add( BOOST_TEST_CASE( &test_ogl_gui_image ),0 );
	test->add( BOOST_TEST_CASE( &test_sdl_gui_image ),0 );

	return test;
}