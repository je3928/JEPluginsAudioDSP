function y = asymmetric_hard_clipper(x, pos_thresh, neg_thresh)
    
    % Calculate length of input signal
    len = length(x);
    
    % Intialise output vector
    y = zeros(1, len);

    % Loop through sample values
    for i = 1:len

        % If input is greater than positive threshold, set output to
        % postive threshold
        if x(i) > pos_thresh

            y(i) = pos_thresh;

        % If input is lower than the minus of the negative threshold, set output to
        % minus of the negative threshold
        elseif x(i) < (-neg_thresh)

            y(i) = (-neg_thresh);
        % Else set output to input
        else

            y(i) = x(i);
        end
    end
end