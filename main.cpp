// main.cpp
#include <gtk/gtk.h>
#include <string>
#include <cstring>
#include "PasswordGenerator.h"

PasswordGenerator* g_generator = nullptr;
GtkWidget* g_login_entry;
GtkWidget* g_password_entry;
GtkWidget* g_result_entry;
GtkWidget* g_length_spin;
GtkWidget* g_show_password_check;

void on_generate_button_clicked(GtkWidget* widget, gpointer data) {
    const gchar* login = gtk_entry_get_text(GTK_ENTRY(g_login_entry));
    const gchar* master = gtk_entry_get_text(GTK_ENTRY(g_password_entry));
    int length = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(g_length_spin));
    
    if (strlen(login) == 0 || strlen(master) == 0) {
        GtkWidget* dialog = gtk_message_dialog_new(
            GTK_WINDOW(data),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "Пожалуйста, введите логин и мастер-пароль!"
        );
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    std::string result = g_generator->generatePassword(login, master, length);
    gtk_entry_set_text(GTK_ENTRY(g_result_entry), result.c_str());
}

void on_copy_button_clicked(GtkWidget* widget, gpointer data) {
    const gchar* password = gtk_entry_get_text(GTK_ENTRY(g_result_entry));
    if (strlen(password) > 0 && strcmp(password, "Нажмите 'Сгенерировать'") != 0) {
        GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
        gtk_clipboard_set_text(clipboard, password, -1);
        
        GtkWidget* dialog = gtk_message_dialog_new(
            GTK_WINDOW(data),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_CLOSE,
            "Пароль скопирован в буфер обмена!"
        );
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void on_clear_button_clicked(GtkWidget* widget, gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(g_login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(g_password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(g_result_entry), "Нажмите 'Сгенерировать'");
}

// Новая функция для показа/скрытия пароля
void on_show_password_toggled(GtkToggleButton* togglebutton, gpointer data) {
    gboolean show = gtk_toggle_button_get_active(togglebutton);
    
    if (show) {
        // Показываем пароль
        gtk_entry_set_visibility(GTK_ENTRY(g_password_entry), TRUE);
        gtk_button_set_label(GTK_BUTTON(togglebutton), "🙈 Скрыть");
    } else {
        // Скрываем пароль
        gtk_entry_set_visibility(GTK_ENTRY(g_password_entry), FALSE);
        gtk_button_set_label(GTK_BUTTON(togglebutton), "👁️ Показать");
    }
}

void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* label;
    GtkWidget* button;
    GtkWidget* hbox_password;
    
    // Создаем окно
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Генератор безопасных паролей");
    gtk_window_set_default_size(GTK_WINDOW(window), 520, 400);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    // Создаем сетку
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    // Заголовок
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), 
        "<span size='x-large' weight='bold'>🔐 Генератор безопасных паролей</span>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);
    
    // Логин
    label = gtk_label_new("Логин:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    
    g_login_entry = gtk_entry_new();
    gtk_widget_set_hexpand(g_login_entry, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(g_login_entry), "Введите ваш логин");
    gtk_grid_attach(GTK_GRID(grid), g_login_entry, 1, 1, 1, 1);
    
    // Мастер-пароль с кнопкой показа
    label = gtk_label_new("Мастер-пароль:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    
    // Создаем горизонтальный контейнер для поля пароля и кнопки
    hbox_password = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_hexpand(hbox_password, TRUE);
    
    g_password_entry = gtk_entry_new();
    gtk_widget_set_hexpand(g_password_entry, TRUE);
    gtk_entry_set_visibility(GTK_ENTRY(g_password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(g_password_entry), "Введите мастер-пароль");
    gtk_box_pack_start(GTK_BOX(hbox_password), g_password_entry, TRUE, TRUE, 0);
    
    // Кнопка показа/скрытия пароля
    g_show_password_check = gtk_toggle_button_new_with_label("👁️ Показать");
    gtk_widget_set_tooltip_text(g_show_password_check, "Показать/скрыть мастер-пароль");
    g_signal_connect(g_show_password_check, "toggled", 
                     G_CALLBACK(on_show_password_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_password), g_show_password_check, FALSE, FALSE, 0);
    
    gtk_grid_attach(GTK_GRID(grid), hbox_password, 1, 2, 1, 1);
    
    // Длина пароля
    label = gtk_label_new("Длина пароля:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    
    GtkWidget* hbox_length = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    g_length_spin = gtk_spin_button_new_with_range(8, 64, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(g_length_spin), 16);
    gtk_widget_set_tooltip_text(g_length_spin, "Длина генерируемого пароля (8-64)");
    gtk_box_pack_start(GTK_BOX(hbox_length), g_length_spin, FALSE, FALSE, 0);
    
    // Добавляем информационную метку о длине
    label = gtk_label_new("(рекомендуется 16-24)");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_label_set_markup(GTK_LABEL(label), "<span size='small' foreground='gray'>рекомендуется 16-24</span>");
    gtk_box_pack_start(GTK_BOX(hbox_length), label, FALSE, FALSE, 0);
    
    gtk_grid_attach(GTK_GRID(grid), hbox_length, 1, 3, 1, 1);
    
    // Кнопки управления
    GtkWidget* button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_CENTER);
    gtk_box_set_spacing(GTK_BOX(button_box), 10);
    
    button = gtk_button_new_with_label("🔑 Сгенерировать");
    gtk_widget_set_tooltip_text(button, "Сгенерировать новый пароль");
    g_signal_connect(button, "clicked", G_CALLBACK(on_generate_button_clicked), window);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("🗑️ Очистить");
    gtk_widget_set_tooltip_text(button, "Очистить все поля");
    g_signal_connect(button, "clicked", G_CALLBACK(on_clear_button_clicked), window);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    gtk_grid_attach(GTK_GRID(grid), button_box, 0, 4, 2, 1);
    
    // Разделитель
    label = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 2, 1);
    
    // Сгенерированный пароль
    label = gtk_label_new("Сгенерированный пароль:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 6, 2, 1);
    
    g_result_entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(g_result_entry), FALSE);
    gtk_widget_set_hexpand(g_result_entry, TRUE);
    gtk_entry_set_text(GTK_ENTRY(g_result_entry), "Нажмите 'Сгенерировать'");
    gtk_widget_set_tooltip_text(g_result_entry, "Сгенерированный безопасный пароль");
    gtk_grid_attach(GTK_GRID(grid), g_result_entry, 0, 7, 2, 1);
    
    // Кнопка копирования
    button = gtk_button_new_with_label("📋 Копировать в буфер");
    gtk_widget_set_tooltip_text(button, "Скопировать пароль в буфер обмена");
    g_signal_connect(button, "clicked", G_CALLBACK(on_copy_button_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 8, 2, 1);
    
    // Информационная строка
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), 
        "<span size='x-small' foreground='gray'>"
        "🔒 Пароль генерируется на основе логина и мастер-пароля. "
        "Никакие данные не сохраняются."
        "</span>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 9, 2, 1);
    
    // Показываем все элементы
    gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
    g_generator = new PasswordGenerator();
    
    GtkApplication* app = gtk_application_new("com.example.password-generator", 
                                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    delete g_generator;
    g_object_unref(app);
    
    return status;
}
