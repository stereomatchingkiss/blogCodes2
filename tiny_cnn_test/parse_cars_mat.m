train_mat = load("cars_train_annos.mat");
disp(train_mat)

function y = write(size, name, data)
    fileID = fopen(name,'w');
    for n = 1:size
        fprintf(fileID, "%d\t%d\t%d\t%d\t%s\n",
	    data.annotations(1,n).bbox_x1, data.annotations(1,n).bbox_x2, 
	    data.annotations(1,n).bbox_y1, data.annotations(1,n).bbox_y2, 
	    data.annotations(1,n).fname
	    );
    end    
    fclose(fileID);
	y = 0;
end


write(8144, 'cars_train_annos.txt', train_mat);

test_mat = load("cars_test_annos.mat");
disp(test_mat)
write(8041, 'cars_test_annos.txt', test_mat);

