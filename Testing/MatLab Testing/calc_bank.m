function [radius, bank_angle, load_factor] = calc_bank(horizontal_airspeed, air_density)
    % Plane Specs
    weight = 20; % newtons
    mass = weight / 9.81; % kg
    coef_lift = 0.75; % Assumed Coef Of Lift used as base case
    wing_area = 0.13575; % meters squared

    lift = coef_lift * 1/2 * air_density * horizontal_airspeed.^2 * wing_area;

    % Calcs
    radius = (mass * horizontal_airspeed.^2) ./...
        sqrt((1/2 * wing_area * air_density * (horizontal_airspeed.^2) * coef_lift).^2 - weight^2);
    bank_angle = atan(sqrt((1/2 * wing_area * air_density * (horizontal_airspeed.^2) * coef_lift).^2 - weight^2) ...
        /weight);
    load_factor = lift / weight;
end
