# Use the official PHP image with Apache
FROM php:8.2-apache

# Set the working directory in the container
WORKDIR /var/www/html

# Install system dependencies
RUN apt-get update && apt-get install -y \
    libsqlite3-dev \
    zip \
    unzip \
    libicu-dev \
    && docker-php-ext-install pdo_sqlite \
    && docker-php-ext-configure intl \
    && docker-php-ext-install intl

# Enable Apache mod_rewrite
RUN a2enmod rewrite

# Install Composer
COPY --from=composer:latest /usr/bin/composer /usr/bin/composer

# Copy application files to the working directory
COPY src /var/www/html

COPY flag.txt /flag.txt

# Set proper permissions for Apache
RUN chown -R www-data:www-data /var/www/html \
    && chmod -R 755 /var/www/html/writable \
    && chmod -R 755 /var/www/html/public

# Install PHP dependencies with Composer
RUN composer update
RUN composer install --no-dev --optimize-autoloader

# Copy the Apache vhost configuration
COPY ./docker/vhost.conf /etc/apache2/sites-available/000-default.conf

# Expose port 80 to the outside world
EXPOSE 80

# Start Apache in the foreground
CMD ["apache2-foreground"]
