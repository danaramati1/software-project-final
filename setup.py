from setuptools import Extension, setup

module = Extension("mykmeanssp", sources=["spkmeansmodule.c", "spkmeans.c", "kmeans.c", "utils.c"])
setup(
    name="mykmeanssp",
    version="1.0.0",
    description="Python wrapper for custom C spkmeans algorithm",
    ext_modules=[module]
)