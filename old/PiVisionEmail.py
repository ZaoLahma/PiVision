#!/usr/bin/python

import smtplib

class PiVisionEmail():
    def __init__(self):
        self.mailServer = smtplib.SMTP('smtp.gmail.com', 587)
        
    def sendMessage(self, address, password, message):
        self.mailServer.starttls()
        ownMailAddress = address
        ownPassword = password
        self.mailServer.login(ownMailAddress, ownPassword)        
        self.mailServer.sendmail(ownMailAddress, ownMailAddress, message)
        self.mailServer.quit()
        
if __name__ == "__main__":
    mail = PiVisionEmail()