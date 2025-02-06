# MyCppApp

<p align="justify">Small C++ API done with Crow, Boost, vcpkg (in manifest mode) and SQLite3.</p>

## Set up the project:
> [!NOTE]
>
> <p align="justify">Any user can set up the project from scratch and run the project locally by following the steps below. The project uses port 8080. Crow and Boost are needed to manages routes and HTTP requests.</p>

1. <p align="justify">Install Visual Studio 2022.</p>

2. <p align="justify">Install C++ Desktop, C++ for Windows, Universal tools for Windows, CMake, Windows SDK and vcpkg in Visual Studio.</p>

3. <p align="justify">Clone the repository:</p>

```
git clone git@github.com:username/MyCppApp.git
```

4. <p align="justify">Enable MSBuild and CMake IDE integration with this copy of vcpkg by using:</p>

```
vcpkg integrate install
```

5. <p align="justify">Visual Studio only supports manifest mode (for now), update the vcpkg.json if needed, or create a new one with:</p>

```
vcpkg new --application
```

6. <p align="justify">Install the packages with vcpkg, crow and boost should be included in the manifest:</p>

```
vcpkg install
```

7. <p align="justify">Right click on CppApp and go to properties, then in vcpkg, verify that "Use Vcpkg Manifest" is set to true.</p>

8. <p align="justify">You may need to add the include folder as additional include folders in the project's C/C++ properties with:</p>

```
$(ProjectDir)include
```s