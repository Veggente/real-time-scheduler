function n = getneighbors(m, G)
%GETNEIGHBORS Get the neighbors for each vertex in a graph
%
%   N = GETNEIGHBORS(M, G) return a cell N that includes all the neighbors
%   for each vertex in the graph G with M vertices.

%TODO: check m in accordance with G
%TODO: check if G is simple and simplify it if necessary

n = cell(1, m); % neighbors for each vertex
for I = 1:size(G, 1)
    n{G(I, 1)} = [n{G(I, 1)}, G(I, 2)];
    n{G(I, 2)} = [n{G(I, 2)}, G(I, 1)];
end

%TODO: sort the neighbors
