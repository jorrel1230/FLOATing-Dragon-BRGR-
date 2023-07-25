function [lift_force, drag_force] = liftdrag(air_dens, air_speed, angle_of_attack)
    % Plane Specs
    wing_area = 0.13575; % meters squared
    epsilon = 0.083766; % Drag from Lift coef
    
    % Aero Coefficients (@ angles of attack)
    lift_coef_0 = 0.171802;
    drag_coef_0 = 0.05499;
    lift_coef_a = 3.78299; % rad^-1

    lift_coef = lift_coef_0 + angle_of_attack * lift_coef_a;
    drag_coef = drag_coef_0 + epsilon * (lift_coef^2);

    dynamic_pressure = (1/2) * air_dens * (air_speed^2);

    lift_force = lift_coef * dynamic_pressure * wing_area;
    drag_force = drag_coef * dynamic_pressure * wing_area;
end

