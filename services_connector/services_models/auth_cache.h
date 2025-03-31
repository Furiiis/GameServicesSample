#pragma once
#include <string>
#include <unordered_map>
#include <random>

namespace Connector {

    class AuthCache {
    public:
        AuthCache() = default;
        ~AuthCache() = default;

        // Получение auth по login
        std::string GetAuthByLogin(const std::string& login) {
            auto it = loginToAuth.find(login);
            if (it == loginToAuth.end()) {
                // Если логина нет, генерируем новый токен
                std::string newAuth = GenerateAuthToken();
                AddPair(login, newAuth);
                return newAuth;
            }
            return it->second;
        }

        // Получение login по auth
        std::string GetLoginByAuth(const std::string& auth) {
            auto it = authToLogin.find(auth);
            if (it == authToLogin.end()) {
                return ""; // Возвращаем пустую строку, если токен не найден
            }
            return it->second;
        }

        // Добавление новой пары login-auth
        void AddPair(const std::string& login, const std::string& auth) {
            loginToAuth[login] = auth;
            authToLogin[auth] = login;
        }

        // Проверка существования login
        bool HasLogin(const std::string& login) const {
            return loginToAuth.find(login) != loginToAuth.end();
        }

        // Проверка существования auth
        bool HasAuth(const std::string& auth) const {
            return authToLogin.find(auth) != authToLogin.end();
        }

    private:
        std::unordered_map<std::string, std::string> loginToAuth;
        std::unordered_map<std::string, std::string> authToLogin;

        std::string GenerateAuthToken() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 9);
            
            std::string token;
            // Первая часть: 15 цифр
            for(int i = 0; i < 15; ++i) {
                token += std::to_string(dis(gen));
            }
            token += "-";
            // Вторая часть: 7 цифр
            for(int i = 0; i < 7; ++i) {
                token += std::to_string(dis(gen));
            }
            token += "-";
            // Третья часть: 6 цифр
            for(int i = 0; i < 6; ++i) {
                token += std::to_string(dis(gen));
            }
            
            return token;
        }
    };

} // namespace Connector 