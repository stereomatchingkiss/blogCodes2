require 'image'
require 'torch'

--[[
Read image info
@param file_name name of the info file
@return name of the images and delta values
@code 
imgs_name, delta = read_imgs_info('info.txt')
print(imgs_name[1][0], imgs_name[1])
for i = 1, 8 do
   print(delta[1][i])
end
@endcode
--]]
function read_imgs_info(file_name)
    local file = io.open(file_name, 'r')
    local imgs_name = {}
    local delta = {}    
    local i = 1    
    while true do
        local line = file:read()
        if line == nil then break end        
        local split_str = {}
        local j = 1
        for str in string.gmatch(line, '%S+') do
            split_str[j] = str            
            j = j + 1
        end
        
        imgs_name[i] = {}
        imgs_name[i][1] = split_str[1]
        imgs_name[i][2] = split_str[2]
        delta[i] = {}
        for j = 1, 8 do
           delta[i][j] = split_str[2+j] 
        end
        i = i + 1        
    end
    
    local real_delta = torch.Tensor(delta, 8):float()
    for i=1, #delta do
        for j=1,8 do
            real_delta[i][j] = delta[i][j]
        end
    end
        
    return imgs_name, real_delta
end    

--[[
Read images in the folder
@param imgs_name This is a "two dimension array" which store the names of a pair of images
@param folder Folder of the images
@param start start index of the imgs_name
@param finish last index of the imgs_name
@return A pair of images
--]]    
function read_imgs(imgs_name, folder, start, finish)
    local output_tensor = torch.Tensor(finish - start + 1, 2, 128, 128)    
    local j = 1
    for i = start, finish do        
        output_tensor[ { {j}, {1}, {}, {} } ] = image.load(folder .. '/' .. imgs_name[i][1], 1, 'byte')
        output_tensor[ { {j}, {2}, {}, {} } ] = image.load(folder .. '/' .. imgs_name[i][2], 1, 'byte')
        j = j + 1
    end    
    
    return output_tensor
end
