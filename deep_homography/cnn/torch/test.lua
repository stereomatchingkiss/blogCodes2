require 'cunn'
require 'nn'
require 'optim'
require 'torch'

require 'squeeze_net'
require 'utils'

cmd = torch.CmdLine()
cmd:text('Options')
cmd:option('-folder','../../data/ms_coco_train_gray_10000','Folder contain images and info.txt')
cmd:option('-model','./squeeze_net_500032/squeeze_homo')
cmd:option('-batch_size',100,'Batch size of testing')

local function euclidean_distance(lhs, rhs)
    local diff = lhs - rhs    
    local square = torch.cmul(diff, diff)
    local sum = 0
    
    if square:nDimension() > 1 then
        sum = torch.sum(square, 2)
        sum = torch.sum(torch.sqrt(sum)/4.0)        
    else
        sum = torch.sum(square)
        sum = torch.sqrt(sum)/4.0
    end
    
    return sum
end

local function test()
        
    local params = cmd:parse(arg)
    local folder = params['folder']
    local imgs_name, delta = read_imgs_info(folder .. '/info.txt')
    
    local img_len = #imgs_name
    local batch_size = params['batch_size'] 
    if( (img_len % batch_size) ~= 0) then
        error()
    end
    
    local net = torch.load(params['model'])    
    local total_step = img_len / batch_size        
    local criterion = nn.AbsCriterion()    
    
    print('total setp:' .. total_step, type(total_step))
    
    ---[[
    net = net:cuda()
    --criterion = criterion:cuda()
    delta = delta:cuda()
                    
    local total_loss = 0    
    local start_time = os.time()
    for i = 1, total_step do
        local offset = ((i-1) * batch_size) % img_len        
        local input_tensor = read_imgs(imgs_name, folder, offset + 1, offset + batch_size):cuda()
        input_tensor = input_tensor / 255.0
        local labels = delta[{ {offset + 1, offset + batch_size}, {} }]
        
        local predict_output = net:forward(input_tensor)
        --local loss = criterion:forward(predict_output, labels)
        local loss = euclidean_distance(predict_output, labels)
        
        total_loss = total_loss + loss
        print(i, ':loss = ', loss / batch_size)        
    end
    
    print('total loss = ', total_loss / total_step)
    
    local end_time = os.time()
    local elapsed_time = os.difftime(end_time - start_time)
    print('spend ' .. elapsed_time .. 's to test')   
    --]]
end    

test()
