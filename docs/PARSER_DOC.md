# Documentação do ConfigParser

 ## Requisitos Obrigatórios
 Para a correta interpretação do arquivo de configuração são necessárias as seguintes regras, caso contrário, exceções são lançadas:
 - O `PATH` passado como parâmetro do construtor deve ser um arquivo com permissão de leitura e não pode ser um diretório.
 - Todas as aspas mais externas, `'` ou `"`, devem estar balanceadas.
 - O arquivo não pode estar vazio.
 - Todo bloco de server teve conter as diretivas `listen` e `root`.
 - Não será permitido a utilização de `locations` aninhados.
 
 ## Regras de sintaxe
 - **Diretivas**: Toda diretiva deve ser terminada com `;`, com exceção de blocos de configuração.
 - **Blocos de Configuração**: São aceitos somente dois tipos de blocos, `server` e `location`, o segundo deve estar contido no primeiro, suas diretivas devem estar contidas entre `{` e `}`.
 - **Valores Duplicados**: Não serão aceitos blocos de configuração com diretivas duplicadas.
 - **Caminhos Duplicados(`location`)**: Não serão aceitos blocos de `location` com o mesmo caminho.
 - **Aspas**: As aspas mais externas são removidas automaticamente e todo o conteúdo interno é preservado.

 ### Diretivas aceitas
 #### Contexto `server`
 - `listen`: Define a interface/IP e porta.
 - `root`: Define o diretório raiz do servidor.
 - `index`: Define os arquivos padrão a serem buscados.
 - `location`: Abre um novo bloco de configuração de rota.
 - `client_max_body_size`: Define o tamanho máximo do corpo da requisição em bytes.
 - `error_page`: Define páginas customizadas para códigos de erro HTTP.
 - `server_name`: Define os nomes do servidor/domínios.
 - `allow_methods`: Lista os métodos HTTP permitidos.
 - `return`: Define um redirecionamento HTTP, exige código de status e URL/caminho.
 - `upload_path`: Define o diretório de destino para uploads de arquivos.

#### Contexto `location`
 - `root`: Define o diretório raiz do servidor.
 - `index`: Define os arquivos padrão a serem buscados.
 - `allow_methods`: Lista os métodos HTTP permitidos.
 - `error_page`: Define páginas customizadas para códigos de erro HTTP.
 - `cgi_extension`: Define a extensão do arquido de cgi.
 - `cgi_path`: Define o caminho do interpretador do cgi.
 - `autoindex`: Booleano que autoriza, ou não, a listagem de diretório.
 - `return`: Define um redirecionamento HTTP, exige código de status e URL/caminho.
 - `upload_path`: Define o diretório de destino para uploads de arquivos.

 ### Regras de diretivas
 #### listen
 É necessário que o ip ou a porta estejam descritos no arquivo. Somente IPV4 será aceito. Caso o ip não esteja no arquivo, seu valor padrão será `0.0.0.0`. Caso a porta não esteja explicitada, seu valor padrão será `80`.
 Exemplos: *listen 80;* ou *listen localhost* 
 #### allow_methods
 Caso `allow_methods` não esteja setado de forma explícita no arquivo, ele assume o valor padrão `GET` `POST` `DELETE`, somente esses três serão aceitos como argumento.
 Exemplos: *allow_methods POST DELETE;* ou *allow_methods GET;*
 #### error_page
 Para utilizar esse diretiva, basta colocar o(s) código(s) de erro e em seguida o caminho para a página de erro. Os códigos de erro devem estar entre 300 e 599.
 Exemplos: *error_page 500 501 502 ./caminho/pagina_erro50x.html;* *error_page 400 ./erro_400.html;*
 #### client_max_body_size
 Coloque o valor numérico e a escala desejada. Somente `K`,`M` e `G` serão aceitos como sufixo, suas variantes em minúsculo também serão. Caso não seja setado seu valor será 2^21. Exemplos: *client_max_body_size 10M*; ou *client_max_body_size 20g*
 #### root
 A diretiva root exige exatamente um argumento válido que define o caminho do diretório raiz. Não pode ser declarada mais de uma vez dentro do mesmo bloco (seja no server ou no location). Exemplos: *root /var/www/html;* ou *root ./site;*
 #### index
 Requer pelo menos um argumento, mas pode receber múltiplos nomes de arquivos que serão buscados sequencialmente.
 Exemplos: *index index.html;* ou *index index.php index.html default.html;*
 #### location
 Abre um bloco de configuração específico para um caminho e exige exatamente um argumento (o caminho da rota), seguido obrigatoriamente por chaves { para iniciar os comandos. Não é permitido declarar múltiplos blocos location com o mesmo caminho exato, nem aninhar um location dentro de outro.
 Exemplos: *location /old-page { ... }* ou *location / { ... }*
 #### server_name
 Define os domínios ou nomes aceitos pelo servidor. Requer pelo menos um argumento, mas suporta receber múltiplos nomes separados por espaço. Exemplos: *server_name localhost;* ou *server_name exemplo1.com exemplo2.com;*
 #### return
 Utilizado para realizar redirecionamentos HTTP. Exige obrigatoriamente dois argumentos: primeiro o código de status HTTP (que deve estar obrigatoriamente entre 300 e 599) e, em seguida, a URL ou caminho de destino. Só pode ser declarado uma vez por bloco. Exemplos: *return 301 /;* ou *return 302 http://google.com;*
 #### upload_path
 Define o diretório de destino onde os arquivos enviados por upload serão salvos. Exige exatamente um argumento e não pode ser duplicado no mesmo bloco. Exemplos: *upload_path ./www/uploads;* ou *upload_path ./uploads;*
 #### cgi_extension e cgi_path
 São diretivas exclusivas do contexto location e atuam sempre em conjunto. cgi_extension define qual extensão ativará o CGI e aceita estritamente os valores .php ou .py. cgi_path define o caminho do binário do interpretador correspondente. Ambas exigem exatamente um argumento e, se uma for declarada no bloco, a outra também deverá obrigatoriamente constar na mesma rota. Exemplos: *cgi_extension .py; cgi_path /usr/bin/python3;*
 #### autoindex
 Ativa ou desativa a listagem de arquivos de um diretório quando nenhum arquivo index for encontrado. Diretiva exclusiva do contexto location. Exige exatamente um argumento, aceitando exclusivamente os valores on ou off.
 Seu valor padrão é off. Exemplos: *autoindex on;* ou *autoindex off;*

 ## Fluxo
 ### 0- Abertura do Arquivo.
 O método `parse(<nome do arquivo>)` é chamado, recebendo o nome do arquivo como parâmetro. Se o parâmetro for um diretório ou um arquivo sem permissão de leitura, uma exceção é lançada, caso contrario uma instância é aberta entre o arquivo e o programa.
 ### 1- Tokenização
 A tokenização é a primeira etapa do processo de configuração, que fica a cargo do método `makeTokens()`, que lê o arquivo de configuração e o converte em blocos chamados tokens. Ele sapara cada palavra em um token.
 #### Tipos de Tokens
 - **`TOKEN_L_BRACE`**: Representa o caractere de chave de abertura `{`.
 - **`TOKEN_R_BRACE`**: Representa o caractere de chave de fechamento `}`.
 - **`TOKEN_SEMICOLON`**: Representa o caractere de ponto e vírgula `;`.
 - **`TOKEN_NEWLINE`**: Representa uma quebra de linha *\n* encontrada no arquivo.
 - **`TOKEN_WORD`**: É o token mais abrangente. Representa qualquer sequência de caracteres válidos que não seja um delimitador especial.
 - **`TOKEN_END`**: Representa o fim absoluto do arquivo.

 #### Aspas
 Afim de permitir uma maior abrangência para o nome de arquivos, foi adicionado no ao makeTokens uma tratativa para as aspas. Imagine que você queira utilizar arquivos com caracteres reservados no nome, como ' ' ou ';', isso é possível passado a palavra entre aspas simples ou duplas. Durante a leitura da configuração elas serão automaticamente removidas e seu conteúdo interno passado inteiramente para um `TOKEN_WORD`, se o conteúdo seja vazio uma exceção é lançada.

 ### 2- Análise Sintática
 Com a lista de tokens montada, o o parser utiliza as métodos de handle para preencher a estrutura. Cada handle representa uma diretiva ou bloco de configuração, eles tem suas próprias regras internas para consumo e validação do token.

 #### lista de handles para server
 - **`handleLocation`**: Responsável por preencher um novo bloco de location.
 - **`handleRoot`**: Define a raiz de onde o server procurará os arquivos.
 - **`handleListen`**: Processa o IP e a porta. Aplica os valores padrão (0.0.0.0 para IP e 80 para porta) caso a configuração esteja omitida parcialmente.
 - **`handleClientMaxBodySize`**: Lê o valor limite da requisição.
 - **`handleErrorPage`**: Mapeia os códigos de erro para uma página específica.
 - **`handleServerName`**: Processa e guarda os nomes de domínio do servidor.
 - **`handleIndex`**: Processa e guarda a lista de arquivos padrão.
 - **`handleReturn`**: Configura o redirecionamento.
 - **`handleUploadPath`**: Define o diretório de upload.

 #### lista de handles para location
 Os que fazem a mesma coisa que os do `server` para o bloco de `location`:
 - **`handleLocationRoot`**
 - **`handleLocationIndex`** 
 - **`handleLocationAllowedMethods`**
 - **`handleLocationErrorPage`**
 - **`handleLocationUploadPath`**
 - **`handleLocationReturn`**

 Os exclusivos de `location`:
 - **`handleLocationAutoindex`**: Processa a permissão de listagem de diretório.
 - **`handleLocationCgiExtension`**: Valida se a extensão da cgi é `.php` ou `.py`.
 - **`handleLocationCgiPath`**: Armazena o caminho do interpretador associado ao cgi.

## Erros
Em caso de erro de parseamento, etapa 2, o método `throw_exception()` é chamado, imprimindo na tela a possível linha em que o erro aconteceu e lançando um exceção
