require 'nn'
require 'torch'

function create_model(input_channels, output_class)
        
    local function conv_block(in_filters, out_filters, kernel_size)        
        local pad = (kernel_size - 1) / 2
        local net = nn.Sequential()
        
        net:add(nn.SpatialConvolution(in_filters, out_filters, kernel_size, kernel_size, 1, 1, pad, pad))
        net:add(nn.SpatialBatchNormalization(out_filters))
        net:add(nn.ReLU())
        
        return net
    end
    
    local function fire_module(in_filters, s1, e1, e3)        
        local net = nn.Sequential()
        net:add(conv_block(in_filters, s1, 1))        
        
        local exp = nn.Concat(2)
        exp:add(conv_block(s1, e1, 1))
        exp:add(conv_block(s1, e3, 3))        
        net:add(exp)
        
        return net        
    end        
    
    net = nn.Sequential()        
    net:add(nn.SpatialConvolution(input_channels, 64, 3, 3, 2, 2, 1, 1))
    net:add(nn.SpatialBatchNormalization(64))
    net:add(nn.ReLU())
    net:add(nn.SpatialMaxPooling(3,3,2,2))
    
    net:add(fire_module(64, 16, 64, 64))
    net:add(fire_module(128, 16, 64, 64))
    net:add(nn.SpatialMaxPooling(3,3,2,2))
    
    net:add(fire_module(128, 32, 128, 128))
    net:add(fire_module(256, 32, 128, 128))
    net:add(nn.SpatialMaxPooling(3,3,2,2))
    
    net:add(fire_module(256, 48, 192, 192))
    net:add(fire_module(192*2, 48, 192, 192))
    net:add(fire_module(192*2, 64, 256, 256))
    net:add(fire_module(512, 64, 256, 256))
    
    net:add(nn.View(512 * 7 * 7))
    net:add(nn.Linear(512 * 7 * 7, output_class))
    
    return net
    
end    
