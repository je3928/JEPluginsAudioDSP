function y = asymmetric_tanh(x, saturation)
    
    % Calculate length of input signal
    len = length(x);

    % Intialise output vector
    y = zeros(1, len);

    % Loop through sample values
    for i = 1:len
        
        % If input is greater than 0, apply soft clipping
        if x(i) > 0

            y(i) = tanh(x(i) * saturation) / tanh(saturation);
        else
        % Else retain original signal, another if statement for the
        % negative part of the signal can be applied to get different
        % clipping on the negative part of the signal. 
            y(i) = x(i);
        end
    end
end