function mn = getmaxneighbors(m, N)
%GETMAXNEIGHBORS Get the maximal schedulable neighbors
%
%   MN = GETMAXNEIGHBORS(M, N) return the set of maximal schedulable
%   neighbors of each vertex with neighbor list N.

% Use recursion with memoization (or dynamic programming)
mn = cell(1, m); % maximal schedulable neighbors
for I = 1:m
    mn{I} = getmaxschedules(N{I}, N); % Get maximal schedules in neighbors of link I
end
