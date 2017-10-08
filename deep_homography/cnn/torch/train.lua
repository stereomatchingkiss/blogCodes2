require 'cunn'
require 'image'
require 'nn'
require 'optim'
require 'torch'
require 'paths'

require 'squeeze_net'
require 'utils'

cmd = torch.CmdLine()
cmd:text('Options')
cmd:option('-folder','../../data/ms_coco_train_gray_10000','Folder contain images and info.txt')
cmd:option('-batch_size',100,'Batch size of training')
cmd:option('-loss_record_step',100,'How many steps the program should record training results')
cmd:option('-total_steps', 500, 'Total training steps')

params = cmd:parse(arg)

folder = params['folder']

local function deep_copy(tbl)
   -- creates a copy of a network with new modules and the same tensors
   local copy = {}
   for k, v in pairs(tbl) do
      if type(v) == 'table' then
         copy[k] = deep_copy(v)
      else
         copy[k] = v
      end
   end
   if torch.typename(tbl) then
      torch.setmetatable(copy, torch.typename(tbl))
   end
   return copy
end

function train(batch_size, loss_record_step, total_steps)
        
    local imgs_name, delta = read_imgs_info(folder .. '/info.txt')
    local net = create_model(2, 8)
    local img_len = #imgs_name
    
    local criterion = nn.MSECriterion()    
    
    net = net:cuda()
    criterion = criterion:cuda()
    delta = delta:cuda()
    
    local params, gradParams = net:getParameters()
    local momentum_val = 0.9
    local optimState = {learningRate = 0.005, momentum = momentum_val}    
    
    local file = io.open('loss_record.txt', 'w')
    
    local function feval(params)
        return criterion.output, gradParams
    end            
            
    local total_loss = 0    
    local start_time = os.time()
    for i = 1, total_steps do             
        gradParams:zero()
        local offset = ((i-1) * batch_size) % img_len
        --print(folder, img_len, offset + 1, offset + batch_size)
        local input_tensor = read_imgs(imgs_name, folder, offset + 1, offset + batch_size):cuda()
        input_tensor = input_tensor / 255.0
        local labels = delta[{ {offset + 1, offset + batch_size}, {} }]
        
        local predict_output = net:forward(input_tensor)
        local loss = criterion:forward(predict_output, labels)
        local dloss = criterion:backward(predict_output, labels)
        net:backward(input_tensor, dloss)
        
        optim.sgd(feval, params, optimState)
                
        total_loss = total_loss + loss                        
        print(i, ':loss = ', loss)        
        if(i % loss_record_step == 0) then
            print(i, ':total loss = ', total_loss/loss_record_step)
            file:write(i .. ':total loss = ' .. total_loss/loss_record_step .. '\n')
            total_loss = 0
            local model = deep_copy(net):float():clearState()
            torch.save('squeeze_homo_' .. i, model)
        end
        
        if i == 30000 then 
            optimState = {learningRate = 0.0005, momentum = momentum_val}            
        elseif i == 60000 then
            optimState = {learningRate = 0.00005, momentum = momentum_val}            
        end                
    end   
    
    local end_time = os.time()
    local elapsed_time = os.difftime(end_time - start_time)
    print('spend ' .. elapsed_time .. ' to train')
    
    net:clearState()
    net = net:float()
    torch.save('squeeze_homo', net)--]]    
    
end    

train(params['batch_size'], params['loss_record_step'], params['total_steps'])
