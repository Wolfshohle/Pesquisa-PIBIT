import socket
import sys

def exibir_tabuleiro(tabuleiro):
    print("\n  0   1   2")
    for i, linha in enumerate(tabuleiro):
        print(f"{i} {linha[0]} | {linha[1]} | {linha[2]}")
        if i < 2:
            print("  ---------")

def main():
    if len(sys.argv) < 2:
        print("Uso: python cliente.py <endereço_do_servidor>")
        return
    
    host = sys.argv[1]
    port = 5000
    
    cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cliente.connect((host, port))
    
    # Recebe símbolo do jogador (X ou O)
    simbolo = cliente.recv(1).decode()
    print(f"Você é o jogador {simbolo}")
    
    try:
        while True:
            # Recebe estado do jogo
            data = cliente.recv(1024).decode()
            if not data:
                break
                
            # Processa estado
            dados = data.split(',')
            tabuleiro = [
                [dados[0], dados[1], dados[2]],
                [dados[3], dados[4], dados[5]],
                [dados[6], dados[7], dados[8]]
            ]
            jogador_atual = int(dados[9])
            
            # Exibe tabuleiro
            exibir_tabuleiro(tabuleiro)
            
            # Verifica se o jogo acabou
            vitoria_x = any(
                all(cell == 'X' for cell in linha) for linha in tabuleiro
            ) or any(
                all(tabuleiro[i][j] == 'X' for i in range(3)) for j in range(3)
            ) or (
                all(tabuleiro[i][i] == 'X' for i in range(3))
            ) or (
                all(tabuleiro[i][2-i] == 'X' for i in range(3))
            )
            
            vitoria_o = any(
                all(cell == 'O' for cell in linha) for linha in tabuleiro
            ) or any(
                all(tabuleiro[i][j] == 'O' for i in range(3)) for j in range(3)
            ) or (
                all(tabuleiro[i][i] == 'O' for i in range(3))
            ) or (
                all(tabuleiro[i][2-i] == 'O' for i in range(3))
            )
            
            empate = all(cell != ' ' for linha in tabuleiro for cell in linha)
            
            if vitoria_x:
                print("Jogador X venceu!")
                break
            elif vitoria_o:
                print("Jogador O venceu!")
                break
            elif empate:
                print("Empate!")
                break
            
            # Verifica se é a vez do jogador
            if (jogador_atual == 0 and simbolo == 'X') or (jogador_atual == 1 and simbolo == 'O'):
                print("Sua vez!")
                while True:
                    try:
                        linha = int(input("Digite a linha (0-2): "))
                        coluna = int(input("Digite a coluna (0-2): "))
                        if 0 <= linha <= 2 and 0 <= coluna <= 2 and tabuleiro[linha][coluna] == ' ':
                            break
                        print("Jogada inválida. Tente novamente.")
                    except ValueError:
                        print("Entrada inválida. Digite números entre 0 e 2.")
                
                # Envia jogada
                cliente.sendall(f"{linha},{coluna}".encode())
            else:
                print("Aguardando jogada do oponente...")
                
    except Exception as e:
        print(f"Erro: {e}")
    finally:
        cliente.close()
        print("Conexão encerrada")

if __name__ == '__main__':
    main()