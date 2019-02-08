import argparse
import os

from random import shuffle

def parse_args():
    parser = argparse.ArgumentParser(description='Split data to train/validate/test set')
    parser.add_argument('--root', type=str, help="Folder contain the folders(each folder has images in it) you want to split to validate and test set")
    parser.add_argument('--test_folder', type=str, help='Folder of test set')
    parser.add_argument('--test_ratio', type=float, default=0.15, help='Ratio of test set')
    parser.add_argument('--validate_folder', type=str, default='', help='Folder of validate set')
    parser.add_argument('--validate_ratio', type=float, default=0.15, help='Ratio of validate set')
						
    return parser.parse_args()
	
args = parse_args()

exts = ['.jpg', '.jpeg', '.png']

def move_files_to(root_dir, file_names, move_to_dir, begin, end):
    print(move_to_dir)
    if end > begin and end is not 0:
        if not os.path.exists(move_to_dir):
            os.makedirs(move_to_dir)
        for i in range(begin, end):            
            os.rename(root_dir + "/" + file_names[i], move_to_dir + "/" + file_names[i])

for dir in os.listdir(args.root):    
    if os.path.isdir(args.root + "/" + dir):
        root_dir = args.root + "/" + dir
        print("moving files in:", root_dir)
        file_names = os.listdir(root_dir)
        shuffle(file_names)        
        test_data_size = len(file_names) * args.test_ratio
        validate_data_size = len(file_names) * args.validate_ratio
		
        test_begin = 0
        test_end = test_begin + test_data_size
        validate_begin = test_end
        validate_end =  validate_begin + validate_data_size
		
        move_files_to(root_dir, file_names, args.test_folder + "/" + dir, int(test_begin) , int(test_end))
        move_files_to(root_dir, file_names, args.validate_folder + "/" + dir, int(validate_begin), int(validate_end))        