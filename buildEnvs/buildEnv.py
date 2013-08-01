import os
 
os.environ["CORAL_PARALLEL"] = "CORAL_PARALLEL_TBB"

os.environ["CORAL_PYTHON_PATH"] = "/usr/bin/python2.7"
os.environ["CORAL_PYTHON_INCLUDES_PATH"] = "/usr/include/python2.7"
os.environ["CORAL_PYTHON_LIB"] = "python2.7"
os.environ["CORAL_PYTHON_LIBS_PATH"] = "/usr/lib"
 
os.environ["CORAL_BOOST_INCLUDES_PATH"] = "/usr/include/boost"
os.environ["CORAL_BOOST_LIBS_PATH"] = "/usr/lib"
os.environ["CORAL_BOOST_PYTHON_LIB"] = "boost_python"
 
os.environ["CORAL_IMATH_INCLUDES_PATH"] = "/usr/include/OpenEXR"
os.environ["CORAL_IMATH_LIBS_PATH"] = "/usr/lib"
os.environ["CORAL_IMATH_LIB"] = "Imath"
os.environ["CORAL_IMATH_IEX_LIB"] = "Iex"
 
os.environ["CORAL_OIIO_INCLUDES_PATH"] = "/usr/include/OpenImageIO"
os.environ["CORAL_OIIO_LIBS_PATH"] = "/usr/lib"
os.environ["CORAL_OIIO_LIB"] = "OpenImageIO"
 
os.environ["CORAL_TBB_INCLUDES_PATH"] = "/usr/include/tbb"
os.environ["CORAL_TBB_LIBS_PATH"] = "/usr/lib"
os.environ["CORAL_TBB_LIB"] = "tbb"
 
os.environ["CORAL_GLEW_INCLUDES_PATH"] = "/usr/include/GL"
os.environ["CORAL_GLEW_LIBS_PATH"] = "/usr/lib"
os.environ["CORAL_GLEW_LIB"] = "libGLEW"
