function [num, G] = unitdiskrandomgraph(n, c, r, m, p, link_mark)
%UNITDISKRANDOMGRAPH Generate a unit-disk random graph
%
%   [NUM, G] = UNITDISKRANDOMGRAPH(N, C, R, M, P) return the conflict graph
%   G of a network with at most M links randomly chosen from the unit-disk
%   random graph with N nodes, communication range C and interference range
%   R. NUM is the actually returned number of links. P indicates plotting.

%TODO: try to be gentle instead of brutal
coordinates = rand(n, 2); % randomly generated coordinates of n nodes
D = squareform(pdist(coordinates, 'euclidean')); % distance matrix of the nodes
links = []; % initial link list
for I = 1:n
    for J = I+1:n
        if D(I, J) < c % the distance is less than c
            links(end+1, :) = [I, J]; % there is a link between I and J
        end
    end
end
numLinks = size(links, 1);
links = links(randperm(numLinks), :); % shuffle 'links'
num = min(numLinks, m); % number of links needed
G = []; % initial edge list for links
for I = 1:num
    for J = I+1:num
        if D(links(I, 1), links(J, 1)) < r ||...
           D(links(I, 1), links(J, 2)) < r ||...
           D(links(I, 2), links(J, 1)) < r ||...
           D(links(I, 2), links(J, 2)) < r % if some two nodes are close
            G(end+1, :) = [I, J]; % there is an edge in the conflict graph
        end
    end
end

if p % plotting set
    %% Plotting the link graph
    figure
    subplot(2, 1, 1)
    hold on
    for I = 1:n
        plot(coordinates(I, 1), coordinates(I, 2), 'o')
    end
    grid on
    for I = 1:num
        n1 = links(I, 1);
        n2 = links(I, 2);
        x = [coordinates(n1, 1), coordinates(n2, 1)];
        y = [coordinates(n1, 2), coordinates(n2, 2)];
        plot(x, y) % the link as a line
    end
    axis([0 1 0 1])
    xlabel('X position')
    ylabel('Y position')
    
    %% Plotting the conflict graph
    subplot(2, 1, 2)
    hold on
    conflictnode = zeros(num, 2); % coordinates for nodes of conflict graph
    for I = 1:num
        n1 = links(I, 1);
        n2 = links(I, 2);
        x = (coordinates(n1, 1)+coordinates(n2, 1))/2;
        y = (coordinates(n1, 2)+coordinates(n2, 2))/2;
        plot(x, y, 'ro')
        if link_mark
            text(x, y, ['\leftarrow', num2str(I)]);
        end
        conflictnode(I, :) = [x, y];
    end
    grid on
    for I = 1:size(G, 1)
        x = [conflictnode(G(I, 1), 1), conflictnode(G(I, 2), 1)];
        y = [conflictnode(G(I, 1), 2), conflictnode(G(I, 2), 2)];
        plot(x, y, 'r')
    end
    axis([0 1 0 1])
    xlabel('X position')
    ylabel('Y position')
end
