# Changelog

This file documents notable changes to this project done before February 2024. For changes after that date, plase refers to the release notes of each release at https://github.com/robotology/idyntree/releases .

## [10.3.0] - 2024-02-07

### Added

- Added getCentroidalRobotLockedInertia and getRobotLockedInertia to KinDynComputations class (https://github.com/robotology/idyntree/pull/1153).

## [10.2.1] - 2024-02-01

### Fixed

- Fixed compilation of IDYNTREE_USES_ASSIMP option in Debian 10 Buster (https://github.com/robotology/idyntree/pull/1148).

## [10.2.0] - 2024-01-12

### Added

- MeshcatVisualizer: Add the possibility to draw lines in MeshcatVisualizer (https://github.com/robotology/idyntree/pull/1141)

## [10.1.0] - 2024-01-09

### Added

- CMake: Permit to explicitly specify Python installation directory by setting the `IDYNTREE_PYTHON_INSTALL_DIR` CMake variable (https://github.com/robotology/idyntree/pull/1124).

- MeshcatVisualizer: Add the possibility to remove a mesh from the visualizer (https://github.com/robotology/idyntree/pull/1138)

### Fixed

- Fixed compilation of pybind11 bindings (https://github.com/robotology/idyntree/pull/1128).
- Fixed support for handling correctly STL files that end with `.STL` in iDynTree Irrlicht-based visualizer (https://github.com/robotology/idyntree/pull/1136).
- Fixed handling meshes with vertix count larger than a limit set in Irrlicht used by idyntree-yarp-visualizer (https://github.com/robotology/idyntree/pull/1139)


## [10.0.0] - 2023-10-16

### Added

- Added the possibility of exporting additional XML elements that are added as child of the `<robot>` element in URDF ModelExporter (https://github.com/robotology/idyntree/pull/1088).
- Added support for reading and wrting joint friction and damping values from URDF files (https://github.com/robotology/idyntree/pull/1094).

### Changed

- Since iDynTree 10, all the changes of iDynTree will go directly to the `master` branch, the `devel` branch will be deleted. Users that want to work with a stable version of iDynTree should use a tagged version of iDynTree .
- The license of the library is changed to `BSD-3-Clause` (https://github.com/robotology/idyntree/pull/1089).
- The `iDynTree::ModelExporterOptions` class was changed to be defined as a struct (https://github.com/robotology/idyntree/pull/1088).
- The header structure has been shortened, from `<iDynTree/@component@/@headername@.h>` to `<iDynTree/@headername@.h>` (https://github.com/robotology/idyntree/pull/1104).
- `iDynTree::idyntree-sensors` library has been merged in `iDynTree::idyntree-model` (https://github.com/robotology/idyntree/pull/1104).
- The `iDynTree::Model` class gained a `m_sensors` attribute and `sensors()` method to access to it to store sensors information as part of the `iDynTree::Model` itself (https://github.com/robotology/idyntree/pull/1106).

### Deprecated

- Linking `iDynTree::idyntree-sensors` is deprecated, you can just link `iDynTree::idyntree-model` instead, or just search and replace `iDynTree::idyntree-sensors` with an empty string if you are already linking `iDynTree::idyntree-model` (https://github.com/robotology/idyntree/pull/1104).
- Including `<iDynTree/Core/@headername@.h>`, `<iDynTree/Model/@headername@.h>`, `<iDynTree/Sensors/@headername@.h>`, `<iDynTree/ModelIO/@headername@.h>`, `<iDynTree/Estimation/@headername@.h>`, `<iDynTree/yarp/@headername@.h>` is deprecated, just include `<iDynTree/@headername@.h>` . To perform this migration, just search and replace `<iDynTree/Core/` with `<iDynTree/`, `<iDynTree/Model/` with `<iDynTree/`, `<iDynTree/Sensors/` with `<iDynTree/`, `<iDynTree/ModelIO/` with `<iDynTree/`, `<iDynTree/Estimation/` with `<iDynTree/`, `<iDynTree/yarp/` with `<iDynTree/` (https://github.com/robotology/idyntree/pull/1104).
- Several methods that take in input both a `iDynTree::Model` and a `iDynTree::SensorsList` have been deprecated, users are suggested to call the variant that takes in input only the `iDynTree::Model`, if necessary by populating correctly the sensors of the `iDynTree::Model` via the `iDynTree::Model::sensors` method (https://github.com/robotology/idyntree/pull/1106).
- Several classes that ended in `Raw` are deprecated, and in their place their non-Raw counterpart should be used. In particular, the pair "deprecated"/"replacement"  is : `iDynTree::PositionRaw`/`iDynTree::Position`, `iDynTree::RotationRaw`/`iDynTree::Rotation`, `iDynTree::RotationalInertiaRaw`/`iDynTree::RotationalInertia` and   `iDynTree::SpatialInertiaRaw`/`iDynTree::SpatialInertia` (https://github.com/robotology/idyntree/pull/1114).


## [9.1.0] - 2023-05-25

### Added

- Added optional dependency on [meshcat-cpp](https://github.com/ami-iit/meshcat-cpp) (https://github.com/robotology/idyntree/pull/1074).
- Added `iDynTree::MeshcatVisualizer` C++ class (https://github.com/robotology/idyntree/pull/1074).

### Fixed

- Fixed the `iDynTree::Visualizer` class to work on macOS, after it was broken in 9.0.0 . To do so, now the compilation with `IDYNTREE_USES_IRRLICHT` set to `ON` on macOS requires irrlicht to be compiled with SDL support (https://github.com/robotology/idyntree/issues/1076, https://github.com/robotology/idyntree/pull/1077).

## [9.0.0] - 2023-05-11

### Fixed
- Fix export of `iDynTree::PrismaticJoint` in `iDynTree::getRandomModel` (https://github.com/robotology/idyntree/pull/1057).

### Changed

- Binary wheels available on PyPI now target `manylinux_2_28`, and the options `IDYNTREE_USES_ASSIMP`, `IDYNTREE_USES_IPOPT` and `IDYNTREE_USES_IRRLICHT` are disable in binary wheels on PyPI. If you need these options enabled, please use conda-forge binaries or build iDynTree from source (https://github.com/robotology/idyntree/pull/1068).
- Compilation with `IDYNTREE_USES_IRRLICHT` set to `ON` now requires `glfw` library. Furthermore, now `IDYNTREE_USES_IRRLICHT`  on Windows requires irrlicht to be compiled with SDL support. This changes have been done to support resizable visualizer windows on Windows (https://github.com/robotology/idyntree/issues/1070, https://github.com/robotology/idyntree/pull/1071).
- Use iDynTree::InverseKinematicsRotationParametrizationRollPitchYaw as default parametrization in iDynTree::InverseKinematics (https://github.com/robotology/idyntree/pull/1058).

#### URDF XML parser change

The XML parser API has changed. Now an additional `XMLParserState` context object is propagated while parsing.
To catch logic errors (which are not pure XML errors that are currently caught by the parser itself) the `XMLParserState` contains a `bool` variable. Further logic can be added to the context state.

The following errors are currently check:
- Duplicate joints in the URDF will cause an error.

### Added

- Build and deploy PyPI wheels for Python 3.10 (https://github.com/robotology/idyntree/pull/1061)

## [8.1.0] - 2023-01-16

### Added
- Enabled the use of irrlicht dependency in PyPI builds (https://github.com/robotology/idyntree/pull/1045).


## [8.0.1] - 2023-01-10

### Fix
- Fix `MatrixView` constructor when `MatrixView<const T>` is built from `MatrixView<T>` (https://github.com/robotology/idyntree/pull/1044).

## [8.0.0] - 2022-12-14

### Added
- Added the possibility to set the alpha channel while loading a model in the meshcat visualizer (https://github.com/robotology/idyntree/pull/1033).
- Add the possibility to pass the list containing the mesh path while building the model (https://github.com/robotology/idyntree/pull/1036).
- Enable implicit conversion and change KinDynComputations holder in the iDynTree bindings (https://github.com/robotology/idyntree/pull/1037).

## [7.0.0] - 2022-08-31

### Fixed
- Fix compilation against PyPy (https://github.com/robotology/idyntree/pull/1018).

### Changed
- estimateExternalWrenches: Avoid to compute the pseudoinverse to evaluate the external wrenches (https://github.com/robotology/idyntree/pull/1017).

## [6.1.0] - 2022-08-18

### Added
- Added creation of sub model object starting from the full model and a sub traversal (https://github.com/robotology/idyntree/pull/1011).

### Fixed
- Fix bug of `InverseKinematics::updateRotationTarget`  that did not actually changed the RotationWeight, but changed the PositionWeight instead (https://github.com/robotology/idyntree/pull/1012).


## [6.0.0] - 2022-07-19

### Added
- Added a new Berdy variant that accounts for estimating the external link wrenches independently of the internal joint torque estimates (https://github.com/robotology/idyntree/pull/991).

### Changed
- Changed signature of the method `BerdyHelper::serializeDynamicVariables` in order to serialize also the `RCM_SENSOR` (https://github.com/robotology/idyntree/pull/991).

## [5.3.0] - 2022-07-19

### Added
- Additional functionalities for pybind11 bindings (https://github.com/robotology/idyntree/pull/1001):
     - `Twist` class
     - `ModelLoader` class
     - Basic kinematics support in `KinDynComputations`

### Changed
- No warning is printed if a sensor not supported by iDynTree is found in an URDF file (https://github.com/robotology/idyntree/pull/997).

### Fixed
- Fix `iDynTreeWrappers.loadReducedModel` when `model_path` argument does not end in `\` (https://github.com/robotology/idyntree/pull/1126).

## [5.2.1] - 2022-05-19

### Fixed
- Fixed compatibility with YARP 3.7  (https://github.com/robotology/idyntree/pull/992).

## [5.2.0] - 2022-05-09

### Added
- Added `DHChain` class to the bindings (https://github.com/robotology/idyntree/pull/984).

### Fixed
- Fixed problems of running two (or more) times the `iDynTree.Visualizer` class on Windows when using SDL >= 1.2.52 (https://github.com/robotology/idyntree/pull/988, https://github.com/robotology/idyntree/pull/986).

## [5.1.0] - 2022-02-25

### Added
- Expose some Rotation conversion functions for pybind11 bindings (https://github.com/robotology/idyntree/pull/973).

### Fixed
- Fixed compatibility at CMake level on macOS with MATLAB R2021a and R2021b (https://github.com/robotology/idyntree/pull/978).

## [5.0.1] - 2022-02-23

### Fixed

- Avoid to use iDynTree material (for example the one specified in URDF) when `.obj` meshes are loaded in the Irrlicht visualizer (https://github.com/robotology/idyntree/pull/974).
- Fix crash on `Visualizer::close` on Windows (https://github.com/robotology/idyntree/pull/976). The fix works only if the used Irrlicht is compiled with SDL support.

## [5.0.0] - 2022-02-08

### Added
- Implement the possibility to use `FromPython` to generate rotation and position in the python
  SWIG bindings starting from arrays (https://github.com/robotology/idyntree/pull/959)

## [4.4.0] - 2022-02-08

### Added
- Implement `idyntree-model-view-meshcat` application (https://github.com/robotology/idyntree/pull/961).

### Fixed
- Fix loading material and color information from URDF files (https://github.com/robotology/idyntree/pull/961).
- Make sure that `pip` is aware of the `idyntree` Python package if iDynTree is installed just via CMake (https://github.com/robotology/idyntree/pull/963).

## [4.3.1] - 2022-01-10

### Fixed
- Fixed `iDynTree::ModelExporter` class and `idyntree-model-simplify-shapes` utility to generate specification-conforming URDF files, by adding huge velocity and effort limits to joints, as these limits are currently not stored inside the `iDynTree::Model` class (https://github.com/robotology/idyntree/pull/957, https://github.com/robotology/idyntree/issues/955).

## [4.3.0] - 2021-11-22

### Added
- Added the possibility to draw in different portions of the visualizer window and textures at the same time. Allow disabling the drawing on textures (https://github.com/robotology/idyntree/pull/903).
- Implement the `operator[]` for `LinkPositions`, `LinkVelArray` and `LinkAccArray` in the swig python bindings (https://github.com/robotology/idyntree/pull/949)
- Expose `ModelTestUtils` in swig bindings (https://github.com/robotology/idyntree/pull/949)
- Define base and joints attributes for `FreeFloatingPos`, `FreeFloatingVel` and `FreeFloatingAcc` in swig bindings (https://github.com/robotology/idyntree/pull/949)

### Deprecated
- The `iDynTree::Visualizer::enviroment()` was deprecated. Please use the `iDynTree::Visualizer::environment()` method instead (https://github.com/robotology/idyntree/pull/903).
- The `iDynTree::idyntree-modelio-urdf` CMake imported target used when using classes such as `iDynTree::ModelLoader` and `iDynTree::ModelExporter`  was deprecated. Please use the `iDynTree::idyntree-modelio` imported target instead.

### Removed
- The tools that depend on YARP (`urdf2dh`, `yarprobotstatepublisher`, `idyntree-sole-gui`, `iDynTreePlotter`) have been moved in [`idyntree-yarp-tools`](https://github.com/robotology/idyntree-yarp-tools) and their have been removed from iDynTree, together with the the option `IDYNTREE_COMPILES_YARP_TOOLS` that was been introduced to disable their compilation in iDynTree 3 (https://github.com/robotology/idyntree/pull/940).

### Fixed
- Fixed the conversion from irrlicht pixels to iDynTree pixels. Fixed a typo in the environment method of the visualizer. Fixed the running of two visualizer instances in the same process (https://github.com/robotology/idyntree/pull/903).
- Fixed the update of the vectors in the visualization that caused the FPS to drop (https://github.com/robotology/idyntree/pull/921).
- Fixed compatibility with pybind11 >= 2.6.0 (https://github.com/robotology/idyntree/issues/867, https://github.com/robotology/idyntree/pull/937).

## [Unreleased]

### Added
- Added support for exporting joint position limits to URDF for 1-DoF joints
  (prismatic and revolute).
- Added pybind11 python bindings for adding and reading joint limits.
- Added the `ModelTransformsSolidShapes.h` header in the `iDynTree::idyntree-solid-shapes` library.
  At the moment, this header contains just the `iDynTree::approximateSolidShapesWithPrimitiveShape`
  function, useful to approximate the solid shapes of a given iDynTree::Model to a series of bounding boxes (https://github.com/robotology/idyntree/pull/941).
- Added `idyntree-model-simplify-shapes` command line tool. This tool is useful to take in input a model, and
  return in output the same model, but with all the geometries of the model approximated with their axis aligned
  bounding boxes (https://github.com/robotology/idyntree/issues/933, https://github.com/robotology/idyntree/pull/941).

### Fixed
- In the URDF exporter, export only frames attached to the exported traversal [#914](https://github.com/robotology/idyntree/pull/914).
- Fixed handling of the AMENT_PREFIX_PATH environment variable (https://github.com/robotology/idyntree/pull/915).
- `__is_mesh` in the `MeshcatVisualizer` now returns `false` for non-meshes, and instead was raising an error (https://github.com/robotology/idyntree/pull/925).

### Deprecated
- The option `IDYNTREE_COMPILES_YARP_TOOLS`, that was already deprecated in iDynTree 3, is now set to `OFF` by default. It will be removed in iDynTree 5, please use the repo https://github.com/robotology/idyntree-yarp-tools instead (https://github.com/robotology/idyntree/pull/919).


## [4.2.0] - 2021-07-23

### Added
- Add the possibility to pass the zmq url to the meshcat visualizer, to simplify its use in [Google Colab](https://colab.research.google.com) (https://github.com/robotology/idyntree/pull/905).

## [4.1.0] - 2021-07-22

### Added
- Add the `MeshcatVisualizer` Python class (https://github.com/robotology/idyntree/pull/901). This class uses [meshcat](https://github.com/rdeits/meshcat-python) to visualize the state of a model either in a browser or in a Jupyter cell. Check the example in `examples/python/MeshcatVisualizerExample.ipynb`.

## [4.0.0] - 2021-07-16

### Added
- Add the visualization of labels in the visualizer (https://github.com/robotology/idyntree/pull/879)

### Removed
- Remove headers `iDynTree/Core/AngularForceVector3.h`, `iDynTree/Core/AngularMotionVector3.h`, `include/iDynTree/Core/ForceVector3.h`, `iDynTree/Core/LinearForceVector3.h`, `include/iDynTree/Core/LinearMotionVector3.h`, `include/iDynTree/Core/MotionVector3.h`. They were deprecated in iDynTree 2.0 (https://github.com/robotology/idyntree/pull/708, https://github.com/robotology/idyntree/pull/885).
- The method `ModelVisualization::getWorldModelTransform()` was removed, it was deprecated in iDynTree 3.0.1 .

## [3.3.1] - 2021-07-16

### Fixed
- Fixed the possibility of installing the bindings of iDynTree (`bindings` directory) on their own, against an already compiled system iDynTree (https://github.com/robotology/idyntree/pull/896).

## [3.3.0] - 2021-07-08

### Added
- Add the `is/asPrismaticJoint` methods in the bindings (https://github.com/robotology/idyntree/issues/881, https://github.com/robotology/idyntree/pull/882)
- Add the possibility of building the binding of iDynTree (`bindings` directory) on their own, against an already compiled system iDynTree (https://github.com/robotology/idyntree/pull/892/, https://github.com/robotology/robotology-superbuild/pull/817).

### Deprecated
- The tools that depend on YARP (`urdf2dh`, `yarprobotstatepublisher`, `idyntree-sole-gui`, `iDynTreePlotter`) have been moved in [`idyntree-yarp-tools`](https://github.com/robotology/idyntree-yarp-tools) and their use in iDynTree has been deprecated. The option `IDYNTREE_COMPILES_YARP_TOOLS` has been introduced to disable their compilation. This option is set by default to `ON` on iDynTree 3, will exist but default to `OFF` in iDynTree 4, and will be removed in iDynTree 5.

## [3.2.1] - 2021-06-07

### Fixed
- The example file `examples/python/KinDynComputationsTutorial.py` has been fixed to work with Python 3 and iDynTree >= 2.0 (https://github.com/robotology/idyntree/pull/873).
- Fix of a bug introduced by https://github.com/robotology/idyntree/pull/845 and present in iDynTree 3.1.0 and 3.2.0 in the visualization of boxes (https://github.com/robotology/idyntree/pull/874)

## [3.2.0] - 2021-05-15

### Changed
- The `SwigRef.m`, `SwigMem.m` and `SwigGet.m` MATLAB files are now named `iDynTreeSwigRef.m`, `iDynTreeSwigMem.m` and `iDynTreeSwigGet.m`. This ensure prevent the possibility that other libraries (such as CasADi) install files with the same name in the MATLAB path with incompatible content (https://github.com/robotology/idyntree/issues/865, https://github.com/robotology/idyntree/pull/868).

## [3.1.0] - 2021-04-23

### Added
- Add the possibility to use `MatrixView` and `Span` as input/output objects for `InverseKinematics` class (https://github.com/robotology/idyntree/pull/822).
- Add the possibility to get frame trasform from the visualizer, and to use frame/link name in place of indices (https://github.com/robotology/idyntree/pull/849).
- Add `IDYNTREE_DETECT_ACTIVE_PYTHON_SITEPACKAGES` CMake option (default value: OFF) to detect and install in the active site-package directory the Python bindings (https://github.com/robotology/idyntree/pull/852).
- Add inverseDynamicsWithInternalJointForceTorques method to KinDynComputations, this method permits to compute the inverse dynamics, but providing as an output the internal joint 6D force/torque for each joint (https://github.com/robotology/idyntree/pull/857).

### Changed
- The wheels uploaded to PyPI are now `manylinux_2_24` compliant (https://github.com/robotology/idyntree/pull/844)

### Fixed
- Dynamics: In RNEA Dynamic Loop, return zero for wrench corresponding to non-existing parent joint of base link (https://github.com/robotology/idyntree/pull/857).
- Fixed compilation when using Eigen 3.4 (https://github.com/robotology/idyntree/pull/861).

## [3.0.2] - 2021-04-11

### Fixed
- Fixed compilation of Python bindings on Windows (https://github.com/robotology/idyntree/pull/843).

## [3.0.1] - 2021-03-11

### Fixed
- Fixed the `IDYNTREE_USES_ASSIMP` option on Windows if `IDYNTREE_USES_YARP` is also enabled (https://github.com/robotology/idyntree/pull/832).
- Fixed the ``OBJ`` meshes visualization in the Visualizer library (https://github.com/robotology/idyntree/pull/833).
- Fixed `ModelVisualization::getWorldLinkTransform()` method (https://github.com/robotology/idyntree/issues/836)
- Only link required MATLAB libraries when compiling iDynTree MATLAB bindings (https://github.com/robotology/idyntree/pull/840).

### Deprecated
- The method `ModelVisualization::getWorldModelTransform()` is deprecated, and will be removed in iDynTree 4.0.

## [3.0.0] - 2021-02-03
### Added
- Add the possibility to plot and update frames in the Matlab visualizer.
- Added ``getFileLocationOnLocalFileSystem`` method in ``ExternalMesh`` that attempts to find the mesh location in the local file system. This is now used by the ``Visualizer`` when loading the robot model (https://github.com/robotology/idyntree/pull/798). This can also be used by the `iDynTreeWrapper.prepareVisualization` MATLAB function, if `meshFilePrefix` is explicitly set to `""` (https://github.com/robotology/idyntree/pull/817).
- Add the possibility to extract submatrix with MatrixView (https://github.com/robotology/idyntree/pull/800)
- Improved the Visualizer library: camera animations and corrections, interface for frames and texture, fix of ``STL`` visualization. These improvements also include mouse control support for the camera, also in the `idyntree-model-view` application (https://github.com/robotology/idyntree/pull/802).

### Changed
- Promoted the functions `computeBoundingBoxFromShape` and `computeBoxVertices` to public in the `idyntree-solid-shapes` library (https://github.com/robotology/idyntree/pull/801).
- The `idyntree-yarp` and `idyntree-icub` libraries are now header-only, and are always installed even if `IDYNTREE_USES_YARP` or `IDYNTREE_USES_ICUB_MAIN` are set to `OFF`, to simplify deployment of the library. The downstream libraries that want to use them need to find and link `YARP` and `ICUB` CMake packages on their own (https://github.com/robotology/idyntree/pull/807).

### Fixed
- Fixed the `IDYNTREE_USES_IRRLICHT` option when `irrlicht` is installed via vcpkg (https://github.com/robotology/idyntree/pull/806).


## [2.0.3] - 2021-02-03

### Fixed
- Fixed use of sphere shape in MATLAB-based Visualizer (https://github.com/robotology/idyntree/pull/796).
- Fixed compilation against conda-forge-provided Ipopt on Windows (https://github.com/robotology/idyntree/pull/793).

## [2.0.2] - 2020-12-04

### Fixed
- Fixed compilation of MATLAB bindings on macOS and Windows (https://github.com/robotology/idyntree/pull/789, https://github.com/robotology/idyntree/pull/790).

## [2.0.1] - 2020-11-24

### Fixed
- Fixed problem in pybind11-based Python bindings (https://github.com/robotology/idyntree/pull/781).

## [2.0.0] - 2020-11-22

### Added
- Added a new CMake option `IDYNTREE_COMPILES_TOOLS` to disable compilation of iDynTree tools.
- Added a `KinDynComputations::getCentroidalTotalMomentumJacobian()` method (https://github.com/robotology/idyntree/pull/706)
- iDynTree now supports build compiled as a shared library also on Windows.
- When used in Python, new iDynTree objects can be constructed from generic iterable objects and NumPy arrays (`*.FromPython`),
  and existing objects can be converted to NumPy arrays (`*.toNumPy`) (https://github.com/robotology/idyntree/pull/726).
- iDynTree Python bindings can now be installed with `pip3 install git+https://github.com/robotology/idyntree.git` (https://github.com/robotology/idyntree/pull/733).
- Implement the MatrixView class (https://github.com/robotology/idyntree/pull/734)
- Add the possibility to use `MatrixView` and `Span` as input/output objects for `KinDynComputations` class (https://github.com/robotology/idyntree/pull/736).
- New Python bindings based on [pybind11](https://github.com/pybind/pybind11).
  They can be compiled by specifying the CMake option `IDYNTREE_USES_PYTHON_PYBIND11`. **Note** that the generated bindings are not
  compatible with the SWIG-generated bindings (e.g. functions have different names). They can be imported as `idyntree.pybind` Python module.

### Fixed
- Fixed bug in `yarprobotstatepublisher` that caused segmentation fault each time an unknown joint name was read from the input joint states topic (https://github.com/robotology/idyntree/pull/719)
- Fixed bug in `CubicSpline()` that causes wrong coefficients calculation when boundary conditions are set (https://github.com/robotology/idyntree/pull/723)

### Changed
- By default  iDynTree is compiled as a shared library also on Windows. The `BUILD_SHARED_LIBS` CMake variable can be used to
  control if iDynTree is built as a shared or a static library.
- The Python method `*.fromPyList` is replaced by `*.FromPython` (https://github.com/robotology/idyntree/pull/726).
- The minimum required CMake version to configure and compile iDynTree is now 3.16 (https://github.com/robotology/idyntree/pull/732).
- The Python package name of the SWIG bindings changed from `iDynTree` to `idyntree.bindings` (https://github.com/robotology/idyntree/pull/733, https://github.com/robotology/idyntree/pull/735). To continue referring to iDynTree classes as `iDynTree.<ClassName>`, you can change your `import iDynTree` statements to `import idyntree.bindings as iDynTree`. Otherwise, you can use `import idyntree.bindings` to refer them as `idyntree.bindings.<ClassName>`.
- Improve the use of `const` keyword  in `KinDynComputations`(https://github.com/robotology/idyntree/pull/736).
- Cleanup size and indices attributes. For consistency with std and Eigen, all sizes and indices have been changed to use std::size_t for unsigned quantities and std::ptrdiff_t for signed quantities. The only exception is the index stored in the triplets of the iDynTree::SparseMatrix data structure, that have been left defined to int for compatibility with Eigen (https://github.com/robotology/idyntree/pull/767).

### Removed
- Remove the CMake option IDYNTREE_USES_KDL and all the classes available when enabling it. They were deprecated in iDynTree 1.0 .
- Remove the semantics related classes. They were deprecated in iDynTree 1.0 .
- Remove unnecessary warning messages from [ModelSensorsTransformers.cpp](https://github.com/robotology/idyntree/blob/master/src/sensors/src/ModelSensorsTransformers.cpp) and [URDFDocument.cpp](https://github.com/robotology/idyntree/blob/master/src/model_io/urdf/src/URDFDocument.cpp) (see [PR 718](https://github.com/robotology/idyntree/pull/718))
- Python2 will not be maintained past 2020 and its support has been dropped (https://github.com/robotology/idyntree/pull/726).
- Remove the need to call `iDynTree.init_helpers()` and `iDynTree.init_numpy_helpers()` from Python (https://github.com/robotology/idyntree/pull/726).
- Remove headers and methods that were deprecated in iDynTree 1.0 (https://github.com/robotology/idyntree/pull/751).

## [1.2.0] - 2020-10-17

### Added
- Added the possibility of reusing an already opened figure with the MATLAB iDynTree Visualizer either if the name coincides or by using gcf.

### Changed
- `SolidShapes.h` public API **changes**. API changes are back compatible, but as the **ABI has changed**, this means a re-compilation of the dependent projects is needed. In details:
    - Added getters and setters to all classes in `SolidShapes.h` (`idyntree-model`). Public attributes are still available for compatibility but are now **deprecated** and will be removed in the next major release of iDynTree (2.x).
    - Added `Material` class in `SolidShapes.h` (`idyntree-model`). The `material` attribute in `SolidShape` is now deprecated. Please use the `color` property in the new `Material` class to maintain the previous behaviour. Note that the old and new properties are completely orthogonal. Ensure the code is consistent with their uses.

### Fixed
- Fixed bug in init() of `SimpleLeggedOdometry` that used an incorrect initial world frame location if used with an additional frame of a link (https://github.com/robotology/idyntree/pull/698).
- Fixed bug that prevented to use iDynTree cmake packages directly from the build directory (https://github.com/robotology/idyntree/pull/728).

## [1.1.0] - 2020-06-08

### Added
- Added a new function to `iDynTreeWrappers` for the function `getWorldTransformsAsHomogeneous`.
- Added functions for having a MATLAB iDynTree Visualizer in `iDynTreeWrappers`. Some time optimization has been performed (https://github.com/robotology/idyntree/issues/659).
- Added `bindings` for `getWorldTransformsAsHomogeneous` function.
- Added function `getWorldTransformsAsHomogeneous` that gives a vector of Matrix4x4 based on a vector of strings containing the frame transforms.
- Added `bindings` for handling `linkSolidShapes` properly (https://github.com/robotology/idyntree/issues/656).
- Added `bindings` for `InverseKinematics` (https://github.com/robotology/idyntree/pull/633).
- Implement `cbegin()` / `cend()` and `begin()` / `end()` methods for `VectorDynSize` and `VectorFixSize` (https://github.com/robotology/idyntree/pull/646).
- Added CI for MacOS with `IDYNTREE_USES_OCTAVE` `ON`

## [1.0.7] - 2020-06-07

### Fixed
- Fixed compilation with assimp installed with apt-get on Ubuntu 20.04 (https://github.com/robotology/idyntree/pull/692, https://github.com/robotology/idyntree/issues/693).
- Fixed compilation with Octave >= 5 (https://github.com/robotology/idyntree/pull/692, https://github.com/robotology/idyntree/pull/677).

## [1.0.6] - 2020-05-06

### Fixed
- Fixed compilation with ipopt installed via vcpkg (https://github.com/robotology/idyntree/pull/689).
- Fixed compilation with Visual Studio 2019 16.6 (https://github.com/robotology/idyntree/pull/672).

## [1.0.5] - 2020-04-03

### Fixed
- Fix find_package(iDynTree) when iDynTree is built with IDYNTREE_USES_ASSIMP ON and BUILD_SHARED_LIBS OFF (https://github.com/robotology/idyntree/pull/667).

## [1.0.4] - 2020-04-02

### Fixed
- Further fix for configuration compilation with Assimp >= 5.0.0 (https://github.com/robotology/idyntree/pull/666).

## [1.0.3] - 2020-04-01

### Fixed
- Fixed configuration and compilation with Assimp >= 5.0.0 (https://github.com/robotology/idyntree/pull/661).
- Fixed runtime errors of the MATLAB bindings on Windows and compatibility with MATLAB 2020a (https://github.com/robotology/idyntree/pull/664).

## [1.0.2] - 2020-02-21

### Fixed
- Remove spurious inclusion of Eigen headers in ExtendedKalmanFilter.h public header, that could create problems when using that header in a downstream project that does not use Eigen (https://github.com/robotology/idyntree/pull/639).
- Added find_dependency(OsqpEigen) and find_dependency(LibXml2) when iDynTree is compiled as a static library, fixing the use of iDynTree on Windows (https://github.com/robotology/idyntree/pull/642).

### Changed
- To reduce the possible unexpected problems, the automatic set of the `IDYNTREE_USES_<pkg>` CMake variable when the `<pkg>` CMake package is available in the system has  been removed for Irrlicht and WORHP, as it was already disabled for ASSIMP and ALGLIB (https://github.com/robotology/idyntree/pull/642). To use this dependencies it is now compulsory to set manually the `IDYNTREE_USES_<pkg>` variable to `ON`.

## [1.0.1] - 2020-01-14

### Fixed
- Change CMake version compatibility from [SameMajorVersion to AnyNewerVersion](https://cmake.org/cmake/help/v3.12/module/CMakePackageConfigHelpers.html#generating-a-package-version-file), as API breakage between major version will be limited, and to avoid breaking the compatibility of any downstream project that request a minimum version of iDynTree as in `find_package(iDynTree 0.11 REQUIRED)` (https://github.com/robotology/idyntree/pull/629).

## [1.0.0] - 2020-01-14

### Added
- Added method to compute the inverse dynamics inertial parameters regressor in KinDynComputations ( https://github.com/robotology/idyntree/pull/480 ).
KinDynComputations finally reached feature parity with respect to DynamicsComputations, that will finally be removed in one of the future iDynTree feature releases.
- Added method to return the convex hull of the constraint on the projection of the center of mass (https://github.com/robotology/idyntree/pull/478).
- Added objects to deal with linear optimal control problems in the optimalcontrol library.
- Added ``OSQP`` interface via ``osqp-eigen`` in the optimalcontrol library.
- Fixed bugs in ``MultipleShooting`` solver in the optimalcontrol library.
- Added few lines of documentation in the optimalcontrol library.
- Added interface for ``ALGLIB`` and ``WORHP`` in the optimalcontrol library.
- Multiple shooting solvers can use hessians of costs and constraints in the optimalcontrol library.
- Taking into account also the sparsity pattern of constraints and dynamical system (both in jacobians and hessians) in the optimalcontrol library.
- Added visualization of vectors in the visualization library.
- Added a SolidShape helper library. This library is part of iDynTree, and is meant
to contain all the algorithms that use in some form the visual and collision geometries of the model,
and so they depend on the Assimp library to load meshes.
- Added an helper function that provides rough estimates of the inertial parameters (mass, first moments of mass,
3d inertia matrix elements) of a robot given the total mass of the robot, and its collisions shapes. While the estimates
provided are quite rough, they can be quite useful at least to provide an expected order of magnitude of the parameters,
to normalize errors or as initial points of a nonlinear optimization procedure.
- Added attitude estimator interface to estimate the orientation of an IMU, given the IMU measurements (https://github.com/robotology/idyntree/pull/516).
- Added `DiscreteExtendedKalmanFilterHelper` base class (https://github.com/robotology/idyntree/pull/516).
- Added `AttitudeMahonyFilter` implementation of an explicit formulation of passive complementary filter over quaternion groups (https://github.com/robotology/idyntree/pull/516).
- Added `AttitudeQuaternionEKF` implementation (https://github.com/robotology/idyntree/pull/516).
- Added `getWorldFrameTransform` implementation in `SimpleLeggedOdometry` class
- Added a new version of `changeFixedFrame` in `SimpleLeggedOdometry`class. This can be used to set a desired homogeneous transformation for the fixed frame
- Added `bindings` for `AttitudeMahonyFilter`, `AttitudeQuaternionEKF`, `DiscreteExtendedKalmanFilterHelper` (https://github.com/robotology/idyntree/pull/522)
- Added basic tests for the Attitude Estimator classes (https://github.com/robotology/idyntree/pull/522)
- Added `DiscreteKalmanFilterHelper` class for an implementation of a discrete, linear time-invariant Kalman Filter  (https://github.com/robotology/idyntree/pull/559)
- Added dynamic reset functionality to `DiscreteExtendedKalmanFilterHelper` class (https://github.com/robotology/idyntree/pull/553)
- Added high-level Matlab/Octave wrappers of the iDyntree bindings (https://github.com/robotology/idyntree/pull/530)
- Added bindings for the class `Span` with the name `DynamicSpan` (https://github.com/robotology/idyntree/pull/522)
- Implement `RPYRightTrivializedDerivativeRateOfChange()` and `RPYRightTrivializedDerivativeInverseRateOfChange()` into `Rotation` class
- Implement left Jacobian and left Jacobian inverse of SO(3) in Rotation class (https://github.com/robotology/idyntree/pull/562)
- Add nameIsValid attribute to iDynTree::SolidShape class.
- Add operator[] method to `iDynTree::VectorDynSize` (https://github.com/robotology/idyntree/pull/596)
- Add operator[] method to `iDynTree::VectorFixSize` (https://github.com/robotology/idyntree/pull/596)
- Implement `getTotalMass()` method for Model class(
- Enable the installation of the `ModelTestUtils.h` file (https://github.com/robotology/idyntree/pull/607)
- Added `iDynTree::ModelExporter` class to export `iDynTree::Model` instances to URDF files (https://github.com/robotology/idyntree/pull/554).
- Added support in the URDF parser to correctly parse the optional name parameter of visual and collision elements.
- Added `iDynTree::ModelCalibrationHelper` to simplify loading a model from file, update its inertial parameters and exporting again to file (https://github.com/robotology/idyntree/pull/576).
- In `yarprobotstatepublisher`, add `tf-prefix` and `jointstates-topic` options for the tf prefixes and ROS topic.
* In `yarprobotstatepublisher`, add `reduced-model` optional parameter to stream only the link transformations to transform server. By default, tranformations from all the frames are streamed to the transform server.

### Changed
- The changelog has been migrated to the format described in https://keepachangelog.com/en/1.0.0/ .
- If the IDYNTREE_USES_YARP option is enabled, the minimum required version of YARP is 3.3 .
- The CMake config files are now installed in ${CMAKE_INSTALL_PREFIX}/lib/cmake/iDynTree also in Windows.
- Updated `iDynTree::ModelLoader` class to load by default models with normalized joint ordering (https://github.com/robotology/idyntree/issues/491).
- In `yarprobotstatepublisher` the model joint positions values are initialized to zero and the joint positions values are updadted in run time if the values are available in ROS topic given through `jointstates-topic` parameter.
- In `yarprobotstatepublisher`, joint size check between model joints and joints in ROS topic given through `jointstates-topic` parameter have been removed.

### Deprecated
- All the classes and methods that end in Semantics are deprecated, and will be removed in iDynTree 2.0, see https://github.com/robotology/idyntree/pull/622 for more info.
- The CMake option IDYNTREE_USES_KDL and all the classes available when enabling it are deprecated, and will be removed in iDynTree 2.0 .
- In `yarprobotstatepublisher`, the `robot` option is deprecated, and replaced by `name-prefix`.

### Fixed
- Fixed missing `DOF_ACCELLERATION` data in dynamic variable cache ordering
(https://github.com/robotology/idyntree/pull/587)
- Fixed compatibility of `Span` with SWIG bindings compilation (https://github.com/robotology/idyntree/pull/522)
- Fixed implementation of `Transform::log()` method (https://github.com/robotology/idyntree/pull/562)
- Fixed implementation of `SpatialMotionVector::exp()` method (https://github.com/robotology/idyntree/pull/562)

## [0.11.2] - 2019-12-12

### Added

- The getFrameAcc method that returns the acceleration of a frame was added to the KinDynComputations class.
  As this method takes in input every time the robot acceleration, it is computationally expensive and
  is not suitable to be used for multiple frames in a tight loop. If you need a computationally convenient
  method to access frame accelerations, please open an issue ( https://github.com/robotology/idyntree/pull/482 ).
- It is now possible to specify a non-zero bias base acceleration as input of the ForwardBiasAccKinematics function.
  This is convenient if the bias acceleration that is being computed is the bias acceleration obtained with the
  MIXED velocity representation ( https://github.com/robotology/idyntree/pull/482 ).


### Fixed

- Fixed cache invalidation bug in the getFrameBiasAcc method of KinDynComputations. The internal
  cache used by getBiasAcc was never updated even if the method setRobotState was called, so the
  getFrameBiasAcc method always returned the bias acceleration corresponding to the first call to setRobotState ( https://github.com/robotology/idyntree/pull/482 ).
- Fixed getBiasAcc method in KinDynComputations to take into account the effect of non-zero and non-parallel
  linear and angular base velocity, described in https://github.com/robotology/idyntree/issues/370 .
- Fixed compilation on 32-bit Windows ( https://github.com/robotology/idyntree/pull/506 )
- Fixed a URDF parser regression introduced in iDynTree 0.11 for which collision geometries were also loaded as visual geometries, and viceversa (https://github.com/robotology/idyntree/issues/497, https://github.com/robotology/idyntree/pull/559).
- Fixed a URDF parser regression introduced in iDynTree 0.11 for which geometry elements without origin tag were not correctly parsed (https://github.com/robotology/idyntree/issues/496, https://github.com/robotology/idyntree/pull/564).


## [0.11.1] - 2018-09-10

### Fixed

- Fix a locale-dependent floating point parsing in the URDF parser (https://github.com/robotology/idyntree/pull/475).
- Fix compatibility with SWIG 2 (https://github.com/robotology/idyntree/pull/476).
- Fix behavior of InverseKinematics if Add***Constraint method are called multiple times (https://github.com/robotology/idyntree/pull/479).


## [0.11.0] - 2018-08-31

### Added
- A new URDF parser was implemented. A side effect of the new parser is that the serialization of the links and joints of models
  parsed from URDF may change. In your code, please use name string (in place of indices) to identify joints and links, or use the
  `iDynTree::ModelLoader` class to load a Model with the specified joint serialization.
- Added library `idyntree-modelio-xml` to parse generic XML files. Support for XSD validation (when parsing from file). This library requires [Gnome libxml2](http://xmlsoft.org).
- Added `toEigen` methods for `Span` and added typedef for `Eigen` maps of vectors.
- Added some typedefs to `VectorDynsize` and `VectorFixSize` classes to enable the use of `make_span` with these objects.
- Added copy operator in `VectorDynSize` and `VectorFixSize` for `Span<double>`.
- Added method to obtain relative Jacobians sparsity pattern.
- Added method to obtain free floating Jacobians sparsity pattern.

### Changed
- iDynTree now official supports the following compilers: Microsoft Visual Studio 2015/2017, GCC >= 5.3 and clang >= 3.8 .
- The C++14 standard language is now used in the project, including in the public headers.
- [libxml2](http://xmlsoft.org) is now a required dependency. Check the docs to see how to install it.
- CMake 3.5 is now required to build iDynTree.
- Improve SpatialInertia documentation ( https://github.com/robotology/idyntree/pull/435 ).
- Changed URDF parser to use `libxml2` instead of TinyXML.

### Deprecated
- The `iDynTree/Sensors/SixAxisFTSensor.h` header has been deprecated in favor of the `iDynTree/Sensors/SixAxisForceTorqueSensor.h`.
- Constraints Jacobian now exploit sparsity pattern


## [0.10.0] - 2018-06-20

## Added

- Added the `iDynTree::Span` class (https://github.com/robotology/idyntree/pull/434), modeled after
  the C++20's `std::span` ( http://en.cppreference.com/w/cpp/container/span ). This class can be used to write
  methods and functions that access a contiguous sequence of objects in memory with a known length.
  This is extremly useful to make sure that `iDynTree` classes are easy to use regardless of which
  vector type the downstream code is using, for example if it is one of `std::vector<double>`,
  `Eigen::VectorXd` or `yarp::sig::Vector`
- In the InverseKinematics library, the Frame Constraints can now be enabled and disabled dynamically ( https://github.com/robotology/idyntree/pull/389 ).
- Addition of iDynTree::SchmittTrigger, iDynTree::ContactStateMachine and iDynTree::BipedFootContactClassifier classes for performing contact state detection using Schmitt trigger based thresholding and biped foot contact classification based on an alternate contact switching pattern reasoning over contact makes used for getting primary foot in contact
  (https://github.com/robotology/idyntree/pull/410 ,   https://github.com/robotology/idyntree/pull/411 ).
- Addition of iDynTree::GravityCompensationHelpers class for computing gravity compensation torques using accelerometer measurements (https://github.com/robotology/idyntree/pull/438)
- Initial improvement of the optimal control library ( https://github.com/robotology/idyntree/pull/442 ). See the inline documentation of the classes for more details, or open an issue
  ( https://github.com/robotology/idyntree/issues/new ) requesting documentation on some specific aspects of the optimal control library.


## Changed
- Since 0.10 release, iDynTree uses C++14 in its headers, and requires GCC >= 5 and Visual Studio >= 2015
  to compile.
- Similar to the YARP policy, any new version of iDynTree will have a consecutive minor version number.
  Development version (the one contained in the `devel` branch before release will be denoted with a
  patch version number greater than 100. The next minor release of iDynTree will be 0.11 .


## [0.8.2] - 2018-06-20

### Fixed

- In the classes devoted to the external force-torque estimation, since https://github.com/robotology/idyntree/pull/343 it is possible to have external forces that are completly estimated from external sensors such as skin sensors. If in a given submodels there are no unknown due to this, the pseudoinverse should be skipped ( https://github.com/robotology/idyntree/pull/443 ) .
- Fix compatibility with YARP 3.


## [0.8.1] - 2017-09-29

### Fixed
- The `toEigen(const SpatialMotionVector & vec)` and `toEigen(const SpatialForceVector & vec)`  inline helpers functions in the `iDynTree/Core/EigenHelpers.h` have been modified to return a **`const`** copy of their content.
While this is technically an API change, it was critical because most of the other `toEigen` methods return an `Eigen::Map` object, and users
expect to be able to use `Eigen` modifiers methods such as `toEigen(...).setZero()`. Enforcing a compilation error in this case will help to prevent subtle bugs.
As this is an `inline` function, this modification does not affect `iDynTree`'s `ABI` ( https://github.com/robotology/idyntree/pull/378 ) .
- The CMake configuration files of the release `0.8.0` were generated with the wrong version `0.7.2`.

## [0.8.0] - 2017-09-25

### Added

- This is the first version of iDynTree for which a changelog was introduced.
- The iDynTree::Model class now supports prismatic joints, using the iDynTree::PrismaticJoint class. Support for prismatic joints was added also to the URDF parser ( https://github.com/robotology/idyntree/pull/269 ).
- Classes and function to convert between chains rappresented by the Denavit Hartnberg convention and iDynTree::Model object have been implemented ( https://github.com/robotology/idyntree/pull/350 ).

### Changed

- The methods and constructors of the iDynTree::RevoluteJoint and iDynTreee::PrismaticJoint classes handling joints have been cleaned up ( https://github.com/robotology/idyntree/pull/339 ).
- The logic to enable/disable dependencies has changed. In particular, now all the dependencies (excluding the legacy dependency on KDL) that are found on the system are enabled. Users can still select manually the dependency that they want to compile using the `IDYNTREE_USES_<dep>` variables ( https://github.com/robotology/idyntree/pull/323 ).
- IPOPT has been added as an optional dependency for the Inverse Kinematics component.
- The Octave bindings have been migrated to use the exact same mex code used for the Matlab bindings ( https://github.com/robotology/idyntree/pull/305 ).

## [0.4.0] - 2017-03-10

### Changed

- The `IVector`, `IRawVector`, `IMatrix` and `IRawMatrix` interfaces have been removed for performance reasons,
  see https://github.com/robotology/idyntree/issues/98#issuecomment-158823148 . If you want to write generic
  code in C++ you can rely on templates, and on Matlab and Python you can rely on the native dynamic type system.

- All the core classes that have a fixed size (`Position`, `Rotation`, `Transform`, `SpatialMotionVector`, etc, etc) are
  not initialized by their empty constructor for performance reasons, see https://github.com/robotology/idyntree/issues/98#issuecomment-158795881 .
  From now on, make sure that initialize them before any use. Most of those classes should have a `zero()` method to
  zero its content, or a `Zero()` static method that return a instance with its content set to zero.
  For ensuring that no regression happened on the iDynTree codebase, a CMake advanced option `IDYNTREE_RUN_VALGRIND_TESTS`
  has been added to make that no use of initialized memory occurs.
