function M = getmaxschedules(S, N)
%GETMAXSCHEDULES Get the maximal schedules
%
%   M = GETMAXSCHEDULES(S, N) returns a cell of all the maximal schedulable
%   sets in S given neighborhood relation N. S is an ordered vector
%   denoting the subset of links. N is the neighborhood list. The return
%   value M is a cell of vectors.

%TODO: check S is a sorted vector
if isempty(S) % convert 1-by-0 empty matrix to 0-by-0 empty matrix
    S = [];
end
%TODO: check N is a valid neighborhood list
%TODO: check consistency in sizes of S and N

persistent cache % 'cache' for memoization
if isempty(cache)
    cache = containers.Map({DataHash([])}, {{[]}}); % initialization
end
if isKey(cache, DataHash(S)) % S is in 'cache'
    M = cache(DataHash(S)); % use value in 'cache' directly
    return
else % S is not in 'cache'
    if isempty(S) % Not happening since 'cache' has been initialized
        disp('Error: empty set not hashed!') %TODO: throw an exception
        M = cell(1); % a cell of empty vector
        return
    elseif length(S) == 1 % S has a single link
        M = cell(1);
        M{1} = S; % put S in M
        cache(DataHash(S)) = M; % update 'cache'
        return
    else % S has size > 1, and we need to look at N
        link = S(1); % 'link' is the first link in S
        nonlink = setdiff(S, link); % delete link from S
        nonneighbors = setdiff(nonlink, N{link}); % find the nonneighbors of 'link'
        maxNonneighbors = getmaxschedules(nonneighbors, N); % the max schedules
        parta = cell(size(maxNonneighbors));
        for I = 1:length(maxNonneighbors)
            parta{I} = [link, maxNonneighbors{I}]; % add 'link' to the beginning of each schedule
        end
        maxNonlink = getmaxschedules(nonlink, N); % the max schedules
        partb = cell(0);
        for I = 1:length(maxNonlink)
            if ~isempty(intersect(N{link}, maxNonlink{I})) % 'maxNonlink{I}' contains a neighbor of 'link'
                partb{end+1} = maxNonlink{I}; % include 'maxNonlink{I}' in 'partb'
            end %TODO: preallocate 'partb' for speed?
        end
        M = [parta, partb]; % add parta and partb up to get M
        cache(DataHash(S)) = M; % update 'cache'
        return
    end
end
