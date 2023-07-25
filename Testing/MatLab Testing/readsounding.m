clear
% Reads An Input Sounding Text File into a X by 4 Matrix

% Text File Path
path = '/Users/jorrelrajan/Desktop/FLOATing Dragon AutoPilot/soundings/predict_01_sound.txt';


% Read File
% Notes: Col Values
% 1: P( mb)    2: HT(ft)   3: DIR(deg)   4: SPD (kts)   5: T(C)
soundingData = readmatrix(path);
soundingData(end-1:end,:) = [] % Removes last 2 rows (Unneccesary text)



