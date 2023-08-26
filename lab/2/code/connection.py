# encoding: utf-8
# Revisión 2019 (a Python 3 y base64): Pablo Ventura
# Copyright 2014 Carlos Bederián
# $Id: connection.py 455 2011-05-01 00:32:09Z carlos $

from constants import *
from command import *

class Connection(object):
    """
    Conexión punto a punto entre el servidor y un cliente.
    Se encarga de satisfacer los pedidos del cliente hasta
    que termina la conexión.
    """

    def __init__(self, socket, directory):
        self.sock = socket
        self.dir = directory
        self.is_connected = True
            
    def handle(self):
        """
        Atiende eventos de la conexión hasta que termina.
        """
        while self.is_connected:
            # Recibe el comando desde el cliente
            command = self.sock.recv(BUFFER_SIZE).decode().strip()
            
            command = self.command_parser(command)
            response = command.executeCommand()
            
            if response == 'quit':
                print("Closing connection...")
                self.is_connected = False
            else:
                print(f"Request: {command}")
            
            self.sock.sendall(response.encode("ascii"))

        self.sock.close()

    def command_parser(self, command):
        """
        Parsea el comando recibido del cliente
        """
        
        parts = command.split(" ")
        action = parts[0].lower()
        args = parts[1:]

        cmd = Command(action, args, self.dir)
        return cmd
