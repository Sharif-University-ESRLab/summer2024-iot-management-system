from django.contrib.auth.models import User
from django.http import JsonResponse



class LoginRequiredMiddleware:
    def __init__(self, get_response):
        self.get_response = get_response

    def __call__(self, request):
        if not (request.path.startswith('/login') or request.path.startswith('/signup')):
            try:
                username = request.GET.get('username', None)
                password = request.GET.get('password', None)
                user = User.objects.get(username=username)
                if user.password == password:
                    response = self.get_response(request)
                    return response
            except:
                print('error')
            return JsonResponse({'error': 'auth error'}, safe=False)

        else:
            response = self.get_response(request)
            return response
