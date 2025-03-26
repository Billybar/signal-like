import os
import logging
from pathlib import Path

class ServerConfig:
    DEFAULT_PORT = 1357

    @staticmethod
    def read_port() -> int:
        """
        Read port from myport.info file.
        If file doesn't exist or is invalid, return default port (1357)

        Returns:
            int: Port number to use
        """
        # Get the directory containing the server code
        server_dir = Path(__file__).parent
        port_file = server_dir / "myport.info"

        try:
            # Read and validate port
            with open(port_file, 'r') as f:
                port = int(f.readline().strip())

                # Check if port is in valid range
                if 1 <= port <= 65535:
                    return port
                else:
                    logging.warning(f"Port {port} out of valid range (1-65535)")
                    return ServerConfig.DEFAULT_PORT

        except FileNotFoundError:
            logging.warning(f"Port file {port_file} not found, using default port {ServerConfig.DEFAULT_PORT}")
            return ServerConfig.DEFAULT_PORT

        except ValueError:
            logging.warning(f"Invalid port number in {port_file}, using default port {ServerConfig.DEFAULT_PORT}")
            return ServerConfig.DEFAULT_PORT

        except Exception as e:
            logging.warning(f"Error reading port file: {e}. Using default port {ServerConfig.DEFAULT_PORT}")
            return ServerConfig.DEFAULT_PORT