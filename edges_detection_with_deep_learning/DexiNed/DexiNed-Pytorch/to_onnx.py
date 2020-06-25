import random # import matplotlib.pyplot as plt

import torch
import torch.nn as nn

from torchvision.transforms import transforms

from model import DexiNet
import os

device = torch.device('cpu')
model = DexiNet().to(device)
model.load_state_dict(torch.load(os.path.join('checkpoints','24/24_model.pth'), map_location=device))
model.eval()


from torch.autograd import Variable
import torch
import onnx

# An example input you would normally provide to your model's forward() method.
input = torch.ones(1, 3, 400, 400)
#print(type(input[..., :3, :400, :400]))
raw_output = model(input)

print(raw_output.shape)

torch.onnx.export(model, input, '24_model.onnx', verbose=False, export_params=True)

print("-------------------------check model---------------------------------------\n")


try:
    onnx_model = onnx.load("24_model.onnx")
    onnx.checker.check_model(onnx_model)
    graph_output = onnx.helper.printable_graph(onnx_model.graph)
    with open("graph_output.txt", mode="w") as fout:
        fout.write(graph_output)	
except:
    print("Something went wrong")
	
	
import onnxruntime
import numpy as np

ort_session = onnxruntime.InferenceSession("24_model.onnx")

def to_numpy(tensor):
    return tensor.detach().cpu().numpy() if tensor.requires_grad else tensor.cpu().numpy()

# compute ONNX Runtime output prediction
ort_inputs = {ort_session.get_inputs()[0].name: to_numpy(input)}
ort_outs = ort_session.run(None, ort_inputs)	

print(ort_outs[0].shape)

# compare ONNX Runtime and PyTorch results
np.testing.assert_allclose(to_numpy(raw_output), ort_outs[0], rtol=1e-03, atol=1e-03)

print("Exported model has been tested with ONNXRuntime, and the result looks good!")