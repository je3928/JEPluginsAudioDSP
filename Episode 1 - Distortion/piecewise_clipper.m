function y = piecewise_clipper(x)

    % Calculate length of input
    len = length(x);

    % Initiliase output vector
    y = zeros(1, len);

    % Loop over sample values
    for i = 1:len 
       
        % Get current sample
        currentsample = x(i);

        % If input is less than -1, set output -(2/3)
        if currentsample <= -1

            y(i) = -(2/3);
        % If input is between -1 and 1, calculate value based on piecewise
        % equation.
        elseif currentsample >= -1 && currentsample <= 1
 
            y(i)  = currentsample - ((currentsample^3)/ 3);

        % If input is greater than 1, set output to (2/3)
        elseif x(i) >= 1

            y(i) = (2/3);

        end 

    end
end