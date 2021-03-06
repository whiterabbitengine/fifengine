#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################


from distutils.core import setup
import os, sys

if sys.platform == 'win32':
	pkg_data = {'fife': ['*.pyd','*.dll'] }
else:
	pkg_data = {'fife': ['*.so'] }


setup(name='fife',
      version='0.3.4rc1',
      description='Flexible Isometric Free Engine',
      url='www.fifengine.net',
      packages = ['fife', 'fife.extensions', 'fife.extensions.pychan', 'fife.extensions.librocket', 'fife.extensions.cegui', 'fife.extensions.pychan.widgets', 'fife.extensions.pychan.dialog', 'fife.extensions.pychan.widgets.ext', 'fife.extensions.serializers' ],
      package_dir = { '': os.path.join('engine','python') },
      package_data = pkg_data,
      data_files = [(os.path.join('lib','site-packages', 'fife'),['AUTHORS','CHANGES', 'LICENSE.md' ,'README.md'])],
      license = 'GNU Lesser General Public License, version 2.1'
      )

