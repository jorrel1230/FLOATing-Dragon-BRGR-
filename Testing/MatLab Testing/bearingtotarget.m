% Gives us a "Goal Bearing" - TRUE Direction aircraft needs to travel in
function bearing = bearingtotarget(lat1, long1, lat2, long2)
    % Convert latitude and longitude to radians
    lat1 = math.radians(lat1);
    long1 = math.radians(long1);
    lat2 = math.radians(lat2);
    long2 = math.radians(long2);

    % Calculate the bearing
    bearing = math.atan2( ...
        sin(long2 - long1) * cos(lat2), ...
        cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(long2 - long1));

    % Convert the bearing to degrees
    bearing = bearing * 180 / pi;

    % Make sure the bearing is positive
    bearing = (bearing + 360); % 360

end