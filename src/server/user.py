from datetime import datetime


class User:
    def __init__(self, user_id: bytes, username: str, public_key: bytes):
        """
        Initialize a new client
        Args:
            user_id (bytes): 16 bytes (128 bit) unique identifier
            username (str): ASCII string representing username (null terminated, max 255 bytes)
            public_key (bytes): 160 bytes public key
        """
        if len(user_id) != 16:
            raise ValueError("ID must be 16 bytes")
        if len(username.encode('ascii')) > 255:
            raise ValueError("Username must be max 255 bytes in ASCII")
        if len(public_key) != 160:
            raise ValueError("Public key must be 160 bytes")

        self.user_id = user_id
        self.username = username
        self.public_key = public_key
        self.last_seen = datetime.now()

    def update_last_seen(self):
        """Update the last seen timestamp to current time"""
        self.last_seen = datetime.now()

    def __str__(self):
        """String representation of the client"""
        return f"Client(username={self.username}, last_seen={self.last_seen})"