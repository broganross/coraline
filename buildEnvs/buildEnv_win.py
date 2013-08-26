import os

# location of the coraline repositiory
coralineRepo = r"C:/reops/coraline"

os.environ["CORAL_PYTHON_INCLUDES_PATH"] = "C:/Python26/include"
os.environ["CORAL_PYTHON_LIB"] = "python26"
os.environ["CORAL_PYTHON_LIBS_PATH"] = "C:/Python26/libs"

os.environ["CORAL_BOOST_INCLUDES_PATH"] = coralineRepo + "/libraries/vs10_x64/boost/include"
os.environ["CORAL_BOOST_LIBS_PATH"] = coralineRepo + "/libraries/vs10_x64/boost/lib"
os.environ["CORAL_BOOST_PYTHON_LIB"] = "boost_python-vc100-mt-1_46_1"

os.environ["CORAL_IMATH_INCLUDES_PATH"] = coralineRepo + "/libraries/vs10_x64/openexr/include"
os.environ["CORAL_IMATH_LIBS_PATH"] = coralineRepo + "/libraries/vs10_x64/openexr/lib"
os.environ["CORAL_IMATH_LIB"] = "Imath"
os.environ["CORAL_IMATH_IEX_LIB"] = "Iex"

os.environ["CORAL_OIIO_INCLUDES_PATH"] = coralineRepo + "/libraries/vs10_x64/openimageio/include"
os.environ["CORAL_OIIO_LIBS_PATH"] = coralineRepo + "/libraries/vs10_x64/openimageio/lib"
os.environ["CORAL_OIIO_LIB"] = "OpenImageIO"

os.environ["CORAL_TBB_INCLUDES_PATH"] = coralineRepo + "/libraries/vs10_x64/tbb/include"
os.environ["CORAL_TBB_LIBS_PATH"] = coralineRepo + "/libraries/vs10_x64/tbb/lib"
os.environ["CORAL_TBB_LIB"] = "tbb"

os.environ["CORAL_GLEW_INCLUDES_PATH"] = coralineRepo + "/libraries/vs10_x64/glew/include"
os.environ["CORAL_GLEW_LIBS_PATH"] = coralineRepo + "/libraries/vs10_x64/glew/lib"
os.environ["CORAL_GLEW_LIB"] = "glew32"

os.environ["CORAL_CL_INCLUDES_PATH"] = coralineRepo + "/libraries/vs10_x64/opencl/include"
os.environ["CORAL_CL_LIBS_PATH"] = coralineRepo + "/libraries/vs10_x64/opencl/lib/x64"
os.environ["CORAL_CL_LIB"] = "OpenCL"

os.environ["CORAL_PARALLEL"] = "CORAL_PARALLEL_TBB"

# sdk setup
os.environ["CORAL_INCLUDES_PATH"] = coralineRepo + "/build/coralStandaloneBuild/sdk/coral/includes"
os.environ["CORALUI_INCLUDES_PATH"] = coralineRepo + "/build/coralStandaloneBuild/sdk/coralUi/includes"
os.environ["CORAL_LIBS_PATH"] = coralineRepo + "/build/coralStandaloneBuild/sdk/coral/libs"
os.environ["CORALUI_LIBS_PATH"] = coralineRepo + "/build/coralStandaloneBuild/sdk/coralUi/libs"