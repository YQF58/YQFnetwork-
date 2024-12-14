#include <windows.h>
#include <string>
#include <sstream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ȫ�ֱ���
HWND hDisplay; // ��ʾ����
std::string currentInput = ""; // ��ǰ����
std::string introduction = "����һ��򵥵Ŀ��ӻ����������� YQFnetwork ����"; // ��������
HWND hButtons[16]; // ��ť�������

// ��ť��ǩ
const char* buttonLabels[] = { "7", "8", "9", "4", "5", "6", "1", "2", "3", "0", "+", "-", "*", "/", "=", "C" };

// ִ�м򵥵���ѧ����
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
        default: return "����";
        }

        std::ostringstream oss;
        oss << result;
        return oss.str();
    } catch (...) {
        return "����";
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

    // ������ʾ��
    SetWindowPos(hDisplay, NULL, margin, margin, width - 2 * margin, displayHeight, SWP_NOZORDER);

    // ������ť
    int x = margin, y = margin + displayHeight + margin;
    for (int i = 0; i < 16; i++) {
        SetWindowPos(hButtons[i], NULL, x, y, buttonSize, buttonSize, SWP_NOZORDER);
        x += buttonSize + margin;
        if ((i + 1) % 4 == 0) {
            x = margin;
            y += buttonSize + margin;
        }
    }

    // ���������ı�
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
        "�򵥼�����",
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
        // ��ʾ��
        hDisplay = CreateWindow("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_RIGHT,
            10, 10, 260, 30, hwnd, NULL, NULL, NULL);

        // ������ť
        for (int i = 0; i < 16; i++) {
            hButtons[i] = CreateWindow("BUTTON", buttonLabels[i], WS_VISIBLE | WS_CHILD,
                0, 0, 50, 50, hwnd, (HMENU)(intptr_t)(1000 + i), NULL, NULL);
        }

        // �����ı�
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

