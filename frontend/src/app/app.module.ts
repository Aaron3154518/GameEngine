import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { MdbModalModule } from 'mdb-angular-ui-kit/modal';
import { FormsModule } from '@angular/forms';

import {
  FaIconLibrary,
  FontAwesomeModule,
} from '@fortawesome/angular-fontawesome';
import { far } from '@fortawesome/free-regular-svg-icons';
import { fas } from '@fortawesome/free-solid-svg-icons';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { ParametersComponent } from './parameters/parameters.component';
import { ParameterComponent } from './parameter/parameter.component';
import { ParameterGroupSearchComponent } from './parameter-group-search/parameter-group-search.component';
import { OnInitDirective } from './directives/on-init.directive';
import { ParametersSearchComponent } from './parameters-search/parameters-search.component';
import { SearchComponent } from './search/search.component';
import { InputComponent } from './search/input/input.component';
import { ContainerDirective } from './directives/container.directive';
import {
  GroupColHeaderComponent,
  GroupListComponent,
  SetColHeaderComponent,
  VarListComponent,
} from './parameter-views/parameter-views.component';
import { ParameterVisualizerComponent } from './parameter-visualizer/parameter-visualizer.component';
import { NgbModule, NgbPopoverModule } from '@ng-bootstrap/ng-bootstrap';

@NgModule({
  declarations: [
    AppComponent,
    ParametersComponent,
    ParameterComponent,
    OnInitDirective,
    ParameterGroupSearchComponent,
    ParametersSearchComponent,
    SearchComponent,
    InputComponent,
    ContainerDirective,
    VarListComponent,
    GroupListComponent,
    GroupColHeaderComponent,
    SetColHeaderComponent,
    ParameterVisualizerComponent,
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    FontAwesomeModule,
    MdbModalModule,
    FormsModule,
    NgbModule,
    NgbPopoverModule,
  ],
  providers: [],
  bootstrap: [AppComponent],
})
export class AppModule {
  constructor(library: FaIconLibrary) {
    // Add multiple icons to the library
    library.addIconPacks(far, fas);
  }
}
