
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

SAMPLING_RATE = 250  # Taxa de amostragem em ms

def plot_main_panel(time_vector, data):
    # Painel geral
    fig, axs = plt.subplots(6, 1, figsize=(10, 18), sharex=True)

    # Aceleração
    axs[0].plot(time_vector, data[:, 1], label='Accel X')
    axs[0].set_ylabel('Aceleração X (g)')
    axs[0].set_title('Aceleração X')

    axs[1].plot(time_vector, data[:, 2], label='Accel Y', color='green')
    axs[1].set_ylabel('Aceleração Y (g)')
    axs[1].set_title('Aceleração Y')

    axs[2].plot(time_vector, data[:, 3], label='Accel Z', color='orange')
    axs[2].set_ylabel('Aceleração Z (g)')
    axs[2].set_title('Aceleração Z')

    # Giroscópio
    axs[3].plot(time_vector, data[:, 4], label='Gyro X', color='red')
    axs[3].set_ylabel('Giro X (deg/s)')
    axs[3].set_title('Giroscópio X')

    axs[4].plot(time_vector, data[:, 5], label='Gyro Y', color='purple')
    axs[4].set_ylabel('Giro Y (deg/s)')
    axs[4].set_title('Giroscópio Y')

    axs[5].plot(time_vector, data[:, 6], label='Gyro Z', color='brown')
    axs[5].set_ylabel('Giro Z (deg/s)')
    axs[5].set_title('Giroscópio Z')
    axs[5].set_xlabel('Tempo (ms)')

    for ax in axs:
        ax.legend()

    plt.tight_layout()
    plt.show()

def plot_data(time_vector, data, channel, channel_name):
    plt.figure(figsize=(10, 6))
    plt.plot(time_vector, data[:, channel])
    plt.title(f'{channel_name}')
    plt.xlabel('Tempo (ms)')
    plt.ylabel('Valor')
    plt.grid()
    plt.show()

if __name__ == "__main__":
   
    # Carregando os dados do arquivo csv
    df = pd.read_csv('data.csv', header=0, delimiter=',')

    # Convertendo os dados para um array numpy
    data = df.values

    #Criando o vetor de tempo
    time_vector = pd.Series(np.arange(0, len(df) * SAMPLING_RATE, SAMPLING_RATE))

    while True:
        print("\nSelecione a plotagem desejada:")
        print("1 - Painel principal")
        print("2 - Gráfico da aceleração X")
        print("3 - Gráfico da aceleração Y")
        print("4 - Gráfico da aceleração Z")
        print("5 - Gráfico do giroscópio X")
        print("6 - Gráfico do giroscópio Y")
        print("7 - Gráfico do giroscópio Z")
        print("8 - Exibir dados brutos (primeiras linhas)")
        print("9 - Sair")

        choice = input("Opção: ")

        if choice == "1":
            plot_main_panel(time_vector, data)
        elif choice == "2":
            plot_data(time_vector, data, 1, 'Aceleração X')
        elif choice == "3":
            plot_data(time_vector, data, 2, 'Aceleração Y')
        elif choice == "4":
            plot_data(time_vector, data, 3, 'Aceleração Z')
        elif choice == "5":
            plot_data(time_vector, data, 4, 'Giroscópio X')
        elif choice == "6":
            plot_data(time_vector, data, 5, 'Giroscópio Y')
        elif choice == "7":
            plot_data(time_vector, data, 6, 'Giroscópio Z')
        elif choice == "8":
            # Exibindo as primeiras linhas dos dados
            print(df.head())
        elif choice == "9":
            print("Saindo...")
            break
        else:
            print("Opção inválida. Tente novamente.")




