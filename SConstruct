import os, sys
from utils.util_scripts.path import path as upath

opts = Options('options.py', ARGUMENTS)
opts.Add(BoolOption('debug',  'Build with debuginfos and without optimisations', 1))
opts.Add(BoolOption('testcases',  'Build testcases in unit_tests', 0))
opts.Add(BoolOption('opengl', 'Compile OpenGL support', 1))
opts.Add(EnumOption('script', 'Enable which script-language backend', 'lua', allowed_values=('none', 'lua')))
opts.Add(BoolOption('lite',   'Build the lite version of the library (used for editor, overrides other settings)', 0))
opts.Add(BoolOption('profile', 'Build with profiling information', 0))
opts.Add(EnumOption('guichan', 'Choose guichan version (default 0.6)', '6', allowed_values=('4','5','6')))
opts.Add(BoolOption('msvcproj',  "Create MSVC project file. If defined, won't build code", 0))
opts.Add(BoolOption('utils',  'Build utilities', 0))
opts.Add(BoolOption('docs',  "Generates static analysis documentation into doc-folder. If defined, won't build code", 0))

env = Environment(options = opts, ENV = {'PATH' : os.environ['PATH']})

Help(opts.GenerateHelpText(env))

dontBuild = env['msvcproj'] or env['docs']

# helper functions
def tryConfigCommand(context, cmd):
	ret = context.TryAction(cmd)[0]
	context.Result(ret)
	if ret:
		context.env.ParseConfig(cmd)
	return ret

def importConfig(config):
	module = __import__(config)
	parts = config.split('.')
	for part in parts[1:]:
		module = getattr(module, part)
	return module

def getPlatformConfig():
	pathparts = ('build', '%s-config' % sys.platform)
	filename = os.path.join(*pathparts)
	sconsfilename = '.'.join(pathparts)
	if os.path.exists(filename + '.py'):
		return importConfig(sconsfilename)
	else:
		print 'no custom platform-config found (searched: %s.py)' % filename
		filename += '-dist'
		sconsfilename += '-dist'
		if os.path.exists(filename + '.py'):
			return importConfig(sconsfilename)
		print 'no platform-config found (searched: %s.py)' % filename
		Exit(1)

# custom checks
def checkPKG(context, name):
	context.Message('Checking for %s (using pkg-config)... ' % name)
	return tryConfigCommand(context, 'pkg-config --libs --cflags \'%s\'' % name)

def checkConf(context, name):
	binary = '%s-config' % name.lower()
	context.Message('Checking for %s (using %s)... ' % (name, binary))
	configcall = '%s --libs --cflags' %binary
	return tryConfigCommand(context, configcall)

def checkSimpleLib(context, liblist, header = '', lang = 'c', required = 1):
	for lib in liblist:
		ret = checkPKG(context, lib)
		if ret:
			return ret

		ret = checkConf(context, lib)
		if ret:
			return ret

		if len(header):
			ret = conf.CheckLibWithHeader(lib, header, lang)
		else:
			ret = conf.CheckLib(lib)

		if ret:
			if not lib in conf.env['LIBS']:
				conf.env.Append(LIBS = [lib])
			return ret

	if required:
		print 'required lib %s not found :(' % lib
		Exit(1)

	return False

if dontBuild:
	Export('env')
	if env['msvcproj']:
		SConscript(['engine/SConscript'])
	if env['docs']:
		_jp = os.path.join
		# should prolly be done using scons builders...
		try:
			print "removing old documentation directories"
			upath('doc/doxygen/html').rmtree()
		except OSError:
			pass
		print "generating new doxygen documentation"
		#os.system('doxygen ' + _jp('doc', 'doxygen', 'doxyfile'))
		print "doxygen documentation created succesfully"

		print "generating directory dependency graph"
		cinc2dot = upath(_jp('utils', 'util_scripts', 'cinclude2dot'))	
		outdir = upath(_jp('doc', 'diagrams'))
		tmpfile = outdir / upath('incdep.dot')
		os.system(str(cinc2dot) + ' --src engine --include engine --merge directory > ' + str(tmpfile))
		# remove references to util
		outlines = []
		for line in tmpfile.lines():
			if line.find('-> "engine/util"') == -1:
				outlines.append(line)
		tmpfile.write_lines(outlines)
		#os.system('dot -Tps ' + str(tmpfile) + ' > ' + str(outdir / upath('incdep.ps')))
		#tmpfile.remove()
		print "directory dependency graph created succesfully"

else:
	platformConfig = getPlatformConfig()
	env = platformConfig.initEnvironment(env)
	conf = Configure(env, 
                     custom_tests = {'checkConf': checkConf, 'checkPKG': checkPKG, 'checkSimpleLib': checkSimpleLib},
					 conf_dir = '#/build/.sconf_temp',
					 log_file = '#/build/config.log')
	
	platformConfig.addExtras(conf)
	env = conf.Finish()

	if sys.platform == "win32":
		env.Append(CPPFLAGS = ['-Wall'])
	else:
		env.Append(CPPFLAGS = ['-Wall', '-Wold-style-cast'])
	
	if env['debug'] == 1:
		env.Append(CPPFLAGS = ['-ggdb', '-O0'])
	else:
		if os.getenv('CXXFLAGS'):
			env.Append(CPPFLAGS = Split(os.environ['CXXFLAGS']))
		else:
			env.Append(CPPFLAGS = ['-O2'])
	
	if env['profile']:
		env.Append(CPPFLAGS = ['-pg'])
		env.Append(LINKFLAGS = ['-pg'])
	
	if env['opengl']:
		env.Append(CPPDEFINES = ['HAVE_OPENGL'])
	
	if env['script'] == 'lua':
		env.Append(CPPDEFINES = ['SCRIPTENGINE_LUA'])
	
	if env['lite']:
		env.Append(CPPDEFINES = ['LITE'])	
	
	env.Append(CPPDEFINES = ['GUICHAN_VERSION='+env['guichan']])
	
	Export('env')
	
	SConscript('engine/SConscript')
	
	if not env['lite']:
		env.Append(LIBS = ['fife'])
		env.Append(LIBPATH = ['#/engine'])
	
		enginefiles = ['engine/main.cpp']
		if sys.platform == 'darwin':
			env.Object('engine/SDLMain.m')
			enginefiles.append('engine/SDLMain.o')
		env.Program('fife_engine', enginefiles)
		
		if env['testcases']:
			SConscript('tests/unit_tests/SConscript')

		if env['utils']:
			SConscript([str(p) for p in upath('utils').walkfiles('SConscript')])


	

# vim: set filetype=python: 
