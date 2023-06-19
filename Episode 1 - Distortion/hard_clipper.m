function y = hard_clipper(x, thresh)
    % Calculate length of input
    len = length(x);

    % Initiliase output vector
    y = zeros(1, len);

    % Loop over sample values
    for i = 1:len
        % If input is greater than threshold, set output to threshold
        if x(i) > thresh
            y(i) = thresh;
        % If input is less than the negative of the threshold, set output
        % to negative of the threshold.
        elseif x(i) < -thresh
            y(i) = -thresh;
        % Else retain input signal values
        else
            y(i) = x(i);
        end
    end
end