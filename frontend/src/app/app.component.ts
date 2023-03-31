import { Component } from '@angular/core';
import { ParameterService } from './services/parameter.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
})
export class AppComponent {
  title = 'frontend';

  constructor(protected parameterService: ParameterService) {}
}
