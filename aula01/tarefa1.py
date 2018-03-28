import requests
import bs4

from concurrent.futures import ThreadPoolExecutor, as_completed

# Pesquise sobre a classe ThreadPoolExecutor e seu método map e faça uma
# versão concorrente do código abaixo.
# Não se esqueça de controlar o acesso a objetos compartilhados entre as threads.

def count_words(s):
    req = requests.get(s)
    pars = bs4.BeautifulSoup(req.text, 'html.parser')
    return len(pars.get_text().split())

if __name__ == '__main__':
    site_list = [
        'http://www.uol.com.br',
        'http://www.terra.com.br',
        'http://www.google.com.br',
        'http://www.bol.com.br',
        'http://www.altavista.com.br',
        'https://www.crummy.com/software/BeautifulSoup/bs4/doc/',
        'http://www.stackoverflow.com',
        'http://www.youtube.com',
        'http://www.insper.edu.br',
        'http://www.python.org',
        'https://docs.python.org/3/library/threading.html',
    ]

    with ThreadPoolExecutor(max_workers=10) as executor:
        word_count = {executor.submit(count_words, s): s for s in site_list}
        for fut in as_completed(word_count):
            print(word_count[fut], '- #words', fut.result())
