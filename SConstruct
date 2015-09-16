import os

buildType = 'debug'

include = '#build/$BUILDTYPE/include'
lib = '#build/$BUILDTYPE/lib'

env = Environment(BUILDTYPE = buildType,
                  CPPPATH = [include],
                  LIBPATH = [lib])

env.Append(CCFLAGS = "-g -std=c++14")
env.Append(CCFLAGS = ["-pedantic"
                      , "-ffunction-sections"
                      , "-Wall"
                      , "-Wextra"
                      , "-Wcast-align"
                      , "-Wcast-qual"
                      , "-Wctor-dtor-privacy"
                      , "-Wdisabled-optimization"
                      , "-Wformat=2"
                      , "-Winit-self"
                      , "-Wno-undefined-inline"
                      , "-Wno-undefined-internal"
                      , "-Wmissing-include-dirs"
                      , "-Wold-style-cast"
                      , "-Woverloaded-virtual"
                      , "-Wredundant-decls"
                      , "-Wshadow"
                      , "-Wsign-conversion"
                      , "-Wsign-promo"
                      , "-Wstrict-overflow=5"
                      , "-Wswitch-default"
                      , "-Wundef"
                      , "-Werror"])

compiler = 'clang++'
#compiler = 'g++'
if 'CXX' in os.environ and os.environ['CXX']:
  compiler = os.environ['CXX']
env.Replace(CXX = compiler)

if compiler[:5] == 'clang':
    env.Append(CCFLAGS = "-stdlib=libc++")
    env.Append(LINKFLAGS = "-lc++")
else:
    env.Append(CCFLAGS = "-Wno-sign-conversion")

env['PROJNAME'] = os.path.basename(Dir('.').srcnode().abspath)
print env['PROJNAME']

Export('env')
env.SConscript('src/SConscript', variant_dir='build/$BUILDTYPE')
