function v = plot_pca( v, labels )

%v = predict(convnet, data);
c = cov(v);
[V,D,W] = eig(c);
v = (V*v')';
b = v(labels == 'ball',:); 
n = v(labels == 'noball',:);

plot(b(:,1),b(:,2),'.'); 
hold on; 
plot(n(:,1),n(:,2),'.'); 
hold off;
end

