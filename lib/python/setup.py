import os
from setuptools import setup

def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

setup(
    name='avalanche',
    version='1.5',
    description='ROOT object dispatcher client',
    author='Andy Mastbaum',
    author_email='mastbaum@hep.upenn.com',
    url='http://github.com/mastbaum/avalanche',
    packages=['avalanche'],
    install_requires = ['pyzmq-static', 'couchdb']
)

