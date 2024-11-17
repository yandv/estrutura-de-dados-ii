import sys
import struct
from tabulate import tabulate


data_types_dict = {
    'int': 4,
    'float': 4,
}

def transform(data_type):
    # se começar com string ou str
    if data_type.startswith('string') or data_type.startswith('str'):
        return {
            'type': 'string',
            'size': int(data_type.split('string-')[-1])
        }
    
    return {
        'type': data_type,
        'size': data_types_dict[data_type]
    }

first_arg = 1
file_path = f'/home/allanmxr/Desktop/Allan/faculdade/estrutura-de-dados-ii/tarefa-3/resources/lefkowitz-a6-idade.txt'

if sys.argv[1].endswith('.txt'):
    file_path = sys.argv[1]
    first_arg = 2

data_types = [transform(data_type) for data_type in sys.argv[first_arg:]]

FORMAT = ''.join([data_type['type'][0] for data_type in data_types])
CHUNK_SIZE = struct.calcsize(FORMAT)

# Crie uma tabela para printar os valores

headers = [f"Campo {i+1}" for i in range(len(data_types))]
data = []

# Ler o arquivo binário
with open(file_path, "rb") as file:
    while chunk := file.read(CHUNK_SIZE):
        if len(chunk) == CHUNK_SIZE:
            data.append(struct.unpack(FORMAT, chunk))

print(tabulate(data, headers=headers, tablefmt='grid'))