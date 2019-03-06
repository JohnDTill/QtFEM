::Assumes CFEM is in the parent directory

..\CFEM.exe "TrussTest" 1
::Verified correct output

..\CFEM.exe "TrussExt" 1
::Verified correct output

..\CFEM.exe "Truss" 1

..\CFEM.exe "HW4_Problem2_natural" 1
::Verified correct output

..\CFEM.exe "HW4_Problem2_essential" 1
::Verified correct output

..\CFEM.exe "BeamDistalForce" 1
::Verified correct output

..\CFEM.exe "BeamDistalMoment" 1
::Verified correct output

..\CFEM.exe "Beam370" 1
::Similar reaction forces- distributed force was implemented as concentrated load

..\CFEM.exe "FrameExt" 1