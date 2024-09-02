<template>
  <div class="auth-container">
    <h1>{{ isSignup ? "Sign Up" : "Log In" }}</h1>
    <form @submit.prevent="handleAuth">
      <div>
        <input
            type="text"
            v-model="username"
            placeholder="Username"
            required
        />
      </div>
      <div>
        <input
            type="password"
            v-model="password"
            placeholder="Password"
            required
        />
      </div>
      <button type="submit">{{ isSignup ? "Sign Up" : "Log In" }}</button>
      <p @click="toggleAuthMode">
        {{ isSignup ? "Already have an account? Log In" : "Don't have an account? Sign Up" }}
      </p>
    </form>
    <p v-if="message">{{ message }}</p>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  data() {
    return {
      username: '',
      password: '',
      isSignup: false,
      message: '',
    };
  },
  methods: {
    toggleAuthMode() {
      this.isSignup = !this.isSignup;
      this.message = '';
    },
    handleAuth() {
      const url = this.isSignup
          ? `http://127.0.0.1:8000/signup/?username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`
          : `http://127.0.0.1:8000/login/?username=${encodeURIComponent(this.username)}&password=${encodeURIComponent(this.password)}`;

      axios
          .get(url)
          .then((response) => {
            if (response.data.success) {
              localStorage.setItem('username', this.username);
              localStorage.setItem('password', this.password);
              this.$emit('auth-success');
            } else {
              this.message = response.data.message;
            }
          })
          .catch((error) => {
            console.error('Error during authentication:', error);
            this.message = 'An error occurred. Please try again.';
          });
    },
  },
};
</script>

<style scoped>
.auth-container {
  max-width: 400px;
  margin: 0 auto;
  padding: 20px;
  background: #fff;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  text-align: center;
}

.auth-container input {
  width: 100%;
  padding: 10px;
  margin: 10px 0;
  border: 1px solid #ccc;
  border-radius: 4px;
}

.auth-container button {
  padding: 10px;
  background-color: #007bff;
  color: #fff;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.auth-container button:hover {
  background-color: #0056b3;
}

.auth-container p {
  cursor: pointer;
  color: #007bff;
}

.auth-container p:hover {
  text-decoration: underline;
}

.auth-container p.message {
  color: red;
}
</style>
