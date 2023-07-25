% Calculates bearing Based on Wind Dir, and "Goal" Bearing
% Goal Bearing - The direction that the glider must travel in
% returns direction the glider must "face" to travel in direction of goal
function bearing = bearingcalc(altitude, air_speed, goal_bearing, soundingMatrix)
    wind_speed = interp1(soundingMatrix(:, 2), soundingMatrix(:, 4), altitude .*3.281,'spline', 0);
    wind_dir = interp1(soundingMatrix(:, 2), soundingMatrix(:, 3), altitude .*3.281,'spline', 0);

    bearing = (180 / pi * (asin((-wind_speed / air_speed) * sind(wind_dir - goal_bearing)))) + goal_bearing;
    if ~isreal(bearing)
        bearing = "Windspeed too high";
    end
end
