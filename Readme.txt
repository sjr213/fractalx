9/25/18

To speed up rendering files containing coordinates of triangles used to build the model can be placed in a folder named "Triangles" within the same path as the executable. 
The files have the form of triangles10_2.triangles where the first digit is the number of iterations and the second the number of Seed Triangles (see the Model dialog vertex page). 
These can be built using the Export Triangles menu item under the Help menu. But the source has to be changed for each different number of iterations and seed triangles. 
See CFractalViewImpl:: void OnExportTriangles().

The seed triangles are used to calculate the model and if they need to be calculated they take almost the same amount of time as the model which can be over an hour. 
Using triangle files speeds up the calculation almost 2x.

The file coords2.bmp should be copied from the Fractalx\res folder to the executable folder to be used in the PositionAngleDlg.cpp.

These files are stored in the folder:D:\code\working_github\fractalx2\OtherDependencies which is not uploaded on GitHub because if file sizes.

Put these in the folders:
D:\code\working_github\fractalx2\FractalX\x64\Debug
D:\code\working_github\fractalx2\FractalX\x64\Release

Packages
DxSupport: directxtk_uwp, XAudio2
DxSupport112: directxtk12_uwp
FractalX: directxtk_uwp, audio2
FractalX12: Xaudio
DxTests: XAudio2, directxtk_uwp