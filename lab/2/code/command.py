import os
from constants import *
from base64 import b64encode

class Response():
    def __init__(self, code, description, message):
        self.code = code 
        self.description = description
        self.message = message


class Command():

    def __init__(self, cmd, args, directory):
        self.cmd = cmd
        self.args = args
        self.directory= directory


    def __str__(self) -> str:
        args = " ".join(self.args)
        return f"{self.cmd} {args}"

    def executeCommand(self):
        """"
        Ejecuta el comando y retorna la respuesta
        """


        if self.cmd == "get_file_listing":
            response = self.handle_get_file_listing()
        elif self.cmd == "get_metadata":
            response = self.handle_get_metadata()
        elif self.cmd == "get_slice":
            response = self.handle_get_slice()
        elif self.cmd == "quit":
            response = self.handle_quit()
        else:
            response = f"{INVALID_COMMAND} {error_messages[INVALID_COMMAND]}{EOL}" 

        return response



    def handle_get_file_listing(self):
        """
        Este comando no recibe argumentos y busca obtener la lista de
        archivos que están actualmente disponibles. El servidor responde
        con una secuencia de líneas terminadas en \r\n, cada una con el
        nombre de uno de los archivos disponible. Una línea sin texto
        indica el fin de la lista.
        """

        if len(self.args) != 0:
            code = INVALID_ARGUMENTS
            response = f"{code} {error_messages[code]}{EOL}{EOL}" 

        else:
            code = CODE_OK
            files = os.listdir(self.directory)
            message = ''.join(file+EOL for file in files)
            response = f"{code} {error_messages[code]}{EOL} {message}{EOL}" 

        return response
    


    def handle_get_metadata(self):
        """
        Este comando recibe un argumento FILENAME especificando un
        nombre de archivo del cual se pretende averiguar el tamaño . El2
        servidor responde con una cadena indicando su valor en bytes.
        """

        if len(self.args) != 1:
            code = INVALID_ARGUMENTS
            response = f"{code} {error_messages[code]}{EOL}" 

        else:
            filename = self.args[0]

            if not os.path.exists(self.directory+filename) or os.path.isdir(self.directory+filename):
                code = FILE_NOT_FOUND
                response = f"{code} {error_messages[code]}{EOL}" 

            else:
                code = CODE_OK
                message = str(os.stat(self.directory+self.args[0]).st_size)
                response = f"{code} {error_messages[code]}{EOL}{message}{EOL}" 
        
        return response
    
    def handle_get_slice(self):
        """"
        Este comando recibe en el argumento FILENAME el nombre de
        archivo del que se pretende obtener un slice o parte. La parte se
        especifica con un OFFSET (byte de inicio) y un SIZE (tamaño de la
        parte esperada, en bytes), ambos no negativos . El servidor
        responde con el fragmento de archivo pedido codificado en
        base64 y un {\r\n}.
        """

        if len(self.args) != 3:
            code = INVALID_ARGUMENTS
            response = f"{code} {error_messages[code]}{EOL}" 

        else: 
            code = CODE_OK
            filename = self.args[0]
            offset = int(self.args[1])
            size = int(self.args[2])
            
            if not  os.path.exists(self.directory+filename) or os.path.isdir(self.directory+filename):
                code = FILE_NOT_FOUND
                response = f"{code} {error_messages[code]}{EOL}" 

            if not offset + size <= os.stat(self.directory+filename).st_size:
                code = BAD_OFFSET
                response = f"{code} {error_messages[code]}{EOL}" 

            else:
                file = open(self.directory+filename, "rb")
                file.seek(offset)
                message = b64encode(file.read(size))
                response = f"{CODE_OK} {error_messages[CODE_OK]}{EOL}{message}{EOL}" 
            
        return response
    

    def handle_quit(self):
        """
        Este comando no recibe argumentos y busca terminar la
        conexión. El servidor responde con un resultado exitoso (0 OK) y
        luego cierra la conexión.
        """
        if len(self.args) != 3:
            code = INVALID_ARGUMENTS
            response = f"{code} {error_messages[code]}{EOL}" 

        return 'quit'
    