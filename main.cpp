#include <windows.h>
#include <string>
#include <sstream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 全局变量
HWND hDisplay; // 显示框句柄
std::string currentInput = ""; // 当前输入
std::string introduction = "这是一款简单的可视化计算器，由 YQFnetwork 开发"; // 介绍文字
HWND hButtons[16]; // 按钮句柄数组

// 按钮标签
const char* buttonLabels[] = { "7", "8", "9", "4", "5", "6", "1", "2", "3", "0", "+", "-", "*", "/", "=", "C" };

// 执行简单的数学运算
std::string CalculateResult(const std::string& input) {
    try {
        std::istringstream iss(input);
        double num1, num2;
        char op;
        iss >> num1 >> op >> num2;

        double result = 0;
        switch (op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/': result = (num2 != 0) ? num1 / num2 : 0; break;
        default: return "错误";
        }

        std::ostringstream oss;
        oss << result;
        return oss.str();
    } catch (...) {
        return "错误";
    }
}

void AdjustLayout(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int margin = 10;
    int buttonSize = (width - 5 * margin) / 4;
    int displayHeight = 30;

    // 调整显示框
    SetWindowPos(hDisplay, NULL, margin, margin, width - 2 * margin, displayHeight, SWP_NOZORDER);

    // 调整按钮
    int x = margin, y = margin + displayHeight + margin;
    for (int i = 0; i < 16; i++) {
        SetWindowPos(hButtons[i], NULL, x, y, buttonSize, buttonSize, SWP_NOZORDER);
        x += buttonSize + margin;
        if ((i + 1) % 4 == 0) {
            x = margin;
            y += buttonSize + margin;
        }
    }

    // 调整介绍文本
    int introHeight = 20;
    SetWindowPos(GetDlgItem(hwnd, 2000), NULL, margin, height - introHeight - margin, width - 2 * margin, introHeight, SWP_NOZORDER);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "CalculatorWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "简单计算器",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 400,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // 显示框
        hDisplay = CreateWindow("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_RIGHT,
            10, 10, 260, 30, hwnd, NULL, NULL, NULL);

        // 创建按钮
        for (int i = 0; i < 16; i++) {
            hButtons[i] = CreateWindow("BUTTON", buttonLabels[i], WS_VISIBLE | WS_CHILD,
                0, 0, 50, 50, hwnd, (HMENU)(intptr_t)(1000 + i), NULL, NULL);
        }

        // 介绍文本
        CreateWindow("STATIC", introduction.c_str(), WS_CHILD | WS_VISIBLE,
            10, 330, 260, 20, hwnd, (HMENU)2000, NULL, NULL);

        AdjustLayout(hwnd);
        break;
    }
    case WM_SIZE:
        AdjustLayout(hwnd);
        break;
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id >= 1000 && id < 1016) {
            const char* label = buttonLabels[id - 1000];

            if (strcmp(label, "=") == 0) {
                std::string result = CalculateResult(currentInput);
                SetWindowText(hDisplay, result.c_str());
                currentInput = "";
            } else if (strcmp(label, "C") == 0) {
                currentInput = "";
                SetWindowText(hDisplay, "");
            } else {
                currentInput += label;
                SetWindowText(hDisplay, currentInput.c_str());
            }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

