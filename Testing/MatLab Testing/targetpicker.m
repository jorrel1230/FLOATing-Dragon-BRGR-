%test code to determine perfered landing zone of glider
%does not take into account speeding up (reducing glide) in case of high wind 


dropPoint = [34.67128, -104.55987];

casenumber = '1';



if strlength(casenumber) == 1
    casenumber = "0" + casenumber;
end

targetTable = readtable("targets\predict_" + casenumber + "_targets.txt");
soundingTable = readtable("soundings\predict_" + casenumber + "_sound.txt");
soundingTable([size(soundingTable), size(soundingTable)-1],:) = [];


weights(1) = flightCostFunction(dropPoint(1), dropPoint(2), targetTable.Longitude_deg_(1), targetTable.Latitude_deg_(1), 30000, soundingTable);
weights(2) = flightCostFunction(dropPoint(1), dropPoint(2), targetTable.Longitude_deg_(2), targetTable.Latitude_deg_(2), 30000, soundingTable);
weights(3) = flightCostFunction(dropPoint(1), dropPoint(2), targetTable.Longitude_deg_(3), targetTable.Latitude_deg_(3), 30000, soundingTable);
weights(4) = flightCostFunction(dropPoint(1), dropPoint(2), targetTable.Longitude_deg_(4), targetTable.Latitude_deg_(4), 30000, soundingTable);
weights(5) = flightCostFunction(dropPoint(1), dropPoint(2), targetTable.Longitude_deg_(5), targetTable.Latitude_deg_(5), 30000, soundingTable);
weights(6) = flightCostFunction(dropPoint(1), dropPoint(2), targetTable.Longitude_deg_(6), targetTable.Latitude_deg_(6), 30000, soundingTable);

[weight, targetIndex] = min(weights);
targetTable.Target(targetIndex)



function xAa = flightCostFunction(long1,lat1,long2,lat2, h1, windTable)
    xA = distance(long1,lat1,long2,lat2);
    distanceTraveled = 0;
    vt = 0;
    height = h1;
    deltaH = 1;
    while distanceTraveled <= norm(xA)

        vt = vt + getwind(height, windTable) * ldApprox(height) ./ v_aApprox(height) .* deltaH;
        distanceTraveled = distanceTraveled + ldApprox(height) * deltaH;
        height = height - deltaH;
    end
    height
    xAa = norm(xA-vt);
end

% distance from two gps coordinates
function x_a = distance(long1,lat1,long2,lat2)
    x_a = [(lat2 - lat1)* pi/180* 6378137, (long2 - long1)*pi/180* 6356752.3141];
end

function wind = getwind(h,lookupTable)
    speed = interp1(lookupTable.HT_FT_, lookupTable.SPD_KTS_, h .*3.281,'spline', 0);
    direction = interp1(lookupTable.HT_FT_, lookupTable.DRCT, h .*3.281,'spline', 0);
    wind = [speed .* sin(direction *pi/180), speed .* cos(direction *pi/180)];
end

function airspeed = v_aApprox(h)
    airspeed = exp(0.09 *(h/1000 + 25)) + 8.6;
end

function ld = ldApprox(h) %m
    ld = 12.6/(1+exp(0.19*(h/1000) -25));
end